#include "OnlineUpdateService.h"

#include <Update.h>
#include <cctype>
#include <esp_crt_bundle.h>
#include <esp_heap_caps.h>
#include <esp_http_client.h>
#include <esp_ota_ops.h>
#include <mbedtls/sha256.h>

#include "AppConfig.h"
#include "BootGuardService.h"
#include "UiText.h"
#include "WiFiApplication.h"

namespace {

constexpr char preferencesNamespace[] = "online-update";
constexpr char pendingKey[] = "pending";
constexpr char automaticKey[] = "auto";
constexpr size_t maximumManifestSize = 2048;
constexpr size_t maximumNotesLength = 200;
constexpr uint32_t restartDelayMs = 1500;
constexpr uint32_t taskStackSize = 10240;
constexpr int maximumRedirects = 3;
constexpr char userAgent[] = "STEP-RGB/" FIRMWARE_VERSION;

// "major.minor.patch", each part 0–999, as one comparable number.
bool parseVersion(const String &text, uint32_t &value)
{
  unsigned major = 0;
  unsigned minor = 0;
  unsigned patch = 0;
  char trailing = '\0';
  if (sscanf(text.c_str(), "%u.%u.%u%c", &major, &minor, &patch, &trailing) != 3
      || major > 999
      || minor > 999
      || patch > 999) {
    return false;
  }
  value = major * 1000000UL + minor * 1000UL + patch;
  return true;
}

// Reads a string value from the flat JSON object that
// tools/publish_firmware.py writes; non-ASCII text arrives as plain UTF-8.
bool jsonString(const String &json, const char *key, String &value)
{
  const String pattern = String('"') + key + "\":";
  int index = json.indexOf(pattern);
  if (index < 0) {
    return false;
  }
  index += pattern.length();
  while (index < static_cast<int>(json.length()) && json[index] == ' ') {
    ++index;
  }
  if (index >= static_cast<int>(json.length()) || json[index] != '"') {
    return false;
  }

  value = "";
  for (++index; index < static_cast<int>(json.length()); ++index) {
    const char character = json[index];
    if (character == '"') {
      return true;
    }
    if (character == '\\' && index + 1 < static_cast<int>(json.length())) {
      const char escaped = json[++index];
      if (escaped == 'u') {
        index += 4;
        value += '?';
      } else if (escaped == 'n' || escaped == 'r' || escaped == 't') {
        value += ' ';
      } else {
        value += escaped;
      }
      continue;
    }
    value += character;
  }
  return false;
}

bool jsonNumber(const String &json, const char *key, uint32_t &value)
{
  const String pattern = String('"') + key + "\":";
  int index = json.indexOf(pattern);
  if (index < 0) {
    return false;
  }
  index += pattern.length();
  while (index < static_cast<int>(json.length()) && json[index] == ' ') {
    ++index;
  }
  uint64_t number = 0;
  bool digits = false;
  while (index < static_cast<int>(json.length())
         && json[index] >= '0'
         && json[index] <= '9') {
    number = number * 10 + static_cast<uint64_t>(json[index] - '0');
    if (number > UINT32_MAX) {
      return false;
    }
    digits = true;
    ++index;
  }
  value = static_cast<uint32_t>(number);
  return digits;
}

bool isSafeFileName(const String &name)
{
  if (name.isEmpty() || name.length() > 80 || !name.endsWith(".bin")) {
    return false;
  }
  for (size_t index = 0; index < name.length(); ++index) {
    const char character = name[index];
    if (!isalnum(static_cast<unsigned char>(character))
        && character != '.'
        && character != '-'
        && character != '_') {
      return false;
    }
  }
  return true;
}

bool isSha256Hex(const String &text)
{
  if (text.length() != 64) {
    return false;
  }
  for (size_t index = 0; index < text.length(); ++index) {
    if (!isxdigit(static_cast<unsigned char>(text[index]))) {
      return false;
    }
  }
  return true;
}

String withDetail(const char *message, const String &detail)
{
  String text(message);
  text += ": ";
  text += detail;
  return text;
}

// Opens an HTTPS GET, verified against the built-in root certificate
// bundle, and follows redirects. Returns nullptr and an error text on failure.
esp_http_client_handle_t openHttps(
  const String &url,
  int64_t &contentLength,
  int &statusCode,
  String &error)
{
  esp_http_client_config_t config = {};
  config.url = url.c_str();
  config.crt_bundle_attach = esp_crt_bundle_attach;
  config.timeout_ms = static_cast<int>(AppConfig::OnlineUpdate::httpTimeoutMs);
  config.user_agent = userAgent;
  config.buffer_size = 2048;
  config.buffer_size_tx = 1024;
  config.keep_alive_enable = false;

  esp_http_client_handle_t client = esp_http_client_init(&config);
  if (client == nullptr) {
    error = "init";
    return nullptr;
  }

  for (int redirects = 0;; ++redirects) {
    const esp_err_t result = esp_http_client_open(client, 0);
    if (result != ESP_OK) {
      error = esp_err_to_name(result);
      esp_http_client_cleanup(client);
      return nullptr;
    }
    contentLength = esp_http_client_fetch_headers(client);
    statusCode = esp_http_client_get_status_code(client);
    const bool redirect = statusCode == 301 || statusCode == 302
      || statusCode == 303 || statusCode == 307 || statusCode == 308;
    if (!redirect || redirects >= maximumRedirects) {
      return client;
    }
    esp_http_client_flush_response(client, nullptr);
    esp_http_client_set_redirection(client);
    esp_http_client_close(client);
  }
}

void closeHttps(esp_http_client_handle_t client)
{
  esp_http_client_close(client);
  esp_http_client_cleanup(client);
}

} // namespace

OnlineUpdateService::OnlineUpdateService(
  WiFiApplication &wifi,
  BootGuardService &bootGuard)
  : wifi_(wifi),
    bootGuard_(bootGuard)
{
}

void OnlineUpdateService::begin()
{
  storageReady_ = preferences_.begin(preferencesNamespace, false);
  if (!storageReady_) {
    Serial.println("ERROR: Online update storage could not be opened.");
    return;
  }

  automaticCheck_ = preferences_.getBool(automaticKey, true);
  const String pending = preferences_.getString(pendingKey, "");
  if (!pending.isEmpty()) {
    if (pending == AppConfig::firmwareVersion) {
      Serial.printf("Online update to %s started successfully.\n", pending.c_str());
    } else {
      // The new firmware reset before it was confirmed, so the bootloader
      // returned to this one.
      std::lock_guard<std::mutex> lock(mutex_);
      rolledBackVersion_ = pending;
      Serial.printf(
        "Firmware %s did not start; running %s again.\n",
        pending.c_str(),
        AppConfig::firmwareVersion);
    }
    preferences_.remove(pendingKey);
  }
}

void OnlineUpdateService::update()
{
  const uint32_t now = millis();
  if (installFinished_.load()) {
    if (!restartScheduled_) {
      String version;
      {
        std::lock_guard<std::mutex> lock(mutex_);
        version = latest_.version;
      }
      // Checked on the next boot to detect a rollback.
      if (storageReady_) {
        preferences_.putString(pendingKey, version);
      }
      restartScheduled_ = true;
      restartAt_ = now + restartDelayMs;
    } else if (static_cast<int32_t>(now - restartAt_) >= 0) {
      Serial.println("Restarting into the downloaded firmware.");
      bootGuard_.prepareForPlannedRestart();
      ESP.restart();
    }
    return;
  }

  const bool connected = wifi_.stationHasIpAddress();
  stationConnected_ = connected;
  if (!connected) {
    connectedSince_ = 0;
    return;
  }
  if (connectedSince_ == 0) {
    connectedSince_ = now == 0 ? 1 : now;
  }
  if (!automaticCheck_
      || taskRunning_.load()
      || now - connectedSince_ < AppConfig::OnlineUpdate::firstCheckDelayMs) {
    return;
  }

  const uint32_t interval = lastCheckFailed_.load()
    ? AppConfig::OnlineUpdate::retryIntervalMs
    : AppConfig::OnlineUpdate::checkIntervalMs;
  if (!checkedOnce_.load() || now - lastCheckAt_.load() >= interval) {
    startJob(Job::check);
  }
}

bool OnlineUpdateService::requestCheck(String &message)
{
  if (busy()) {
    message = UI_TEXT("Başka bir güncelleme işlemi sürüyor.");
    return false;
  }
  if (!wifi_.stationHasIpAddress()) {
    message = UI_TEXT("STA ağına bağlı değil; güncelleme denetimi için internet bağlantısı gerekiyor.");
    return false;
  }
  if (!startJob(Job::check)) {
    message = UI_TEXT("Güncelleme işlemi başlatılamadı.");
    return false;
  }
  message = UI_TEXT("Güncelleme denetimi başlatıldı.");
  return true;
}

bool OnlineUpdateService::requestInstall(String &message)
{
  if (busy()) {
    message = UI_TEXT("Başka bir güncelleme işlemi sürüyor.");
    return false;
  }
  if (!updateAvailable_.load()) {
    message = UI_TEXT("Kurulacak yeni sürüm yok. Önce güncellemeleri denetleyin.");
    return false;
  }
  if (!wifi_.stationHasIpAddress()) {
    message = UI_TEXT("STA ağına bağlı değil; güncelleme denetimi için internet bağlantısı gerekiyor.");
    return false;
  }
  if (Update.isRunning()) {
    message = UI_TEXT("Başka bir güncelleme işlemi sürüyor.");
    return false;
  }
  if (!startJob(Job::install)) {
    message = UI_TEXT("Güncelleme işlemi başlatılamadı.");
    return false;
  }
  message = UI_TEXT("Güncelleme indiriliyor.");
  return true;
}

void OnlineUpdateService::setAutomaticCheck(bool enabled)
{
  automaticCheck_ = enabled;
  if (storageReady_) {
    preferences_.putBool(automaticKey, enabled);
  }
}

bool OnlineUpdateService::busy() const
{
  return taskRunning_.load() || installFinished_.load();
}

OnlineUpdateStatus OnlineUpdateService::status() const
{
  OnlineUpdateStatus result;
  result.state = state_.load();
  result.updateAvailable = updateAvailable_.load();
  result.automaticCheck = automaticCheck_;
  result.stationConnected = stationConnected_.load();
  result.progress = progress_.load();
  if (checkedOnce_.load()) {
    result.secondsSinceCheck = static_cast<int32_t>((millis() - lastCheckAt_.load()) / 1000UL);
  }
  std::lock_guard<std::mutex> lock(mutex_);
  result.latestVersion = latest_.version;
  result.notes = latest_.notes;
  result.size = latest_.size;
  result.message = message_;
  result.rolledBackVersion = rolledBackVersion_;
  return result;
}

void OnlineUpdateService::taskEntry(void *context)
{
  OnlineUpdateService &service = *static_cast<OnlineUpdateService *>(context);
  if (service.job_ == Job::install) {
    service.runInstall();
  } else {
    service.runCheck();
  }
  service.taskRunning_ = false;
  vTaskDelete(nullptr);
}

bool OnlineUpdateService::startJob(Job job)
{
  bool expected = false;
  if (!taskRunning_.compare_exchange_strong(expected, true)) {
    return false;
  }
  job_ = job;
  // Core 0 runs Wi-Fi; the Arduino loop with the portal stays on core 1.
  if (xTaskCreatePinnedToCore(
        &OnlineUpdateService::taskEntry,
        "online-update",
        taskStackSize,
        this,
        1,
        nullptr,
        0) != pdPASS) {
    taskRunning_ = false;
    return false;
  }
  return true;
}

void OnlineUpdateService::setState(OnlineUpdateState state, const String &message)
{
  {
    std::lock_guard<std::mutex> lock(mutex_);
    message_ = message;
  }
  state_ = state;
}

bool OnlineUpdateService::fetchManifest(Manifest &manifest, String &error)
{
  if (heap_caps_get_free_size(MALLOC_CAP_INTERNAL) < AppConfig::OnlineUpdate::minimumFreeHeap) {
    error = UI_TEXT("Yetersiz bellek; güncelleme sunucusuna bağlanılamadı.");
    return false;
  }

  String url(AppConfig::OnlineUpdate::repositoryBaseUrl);
  url += AppConfig::OnlineUpdate::manifestFile;
  int64_t contentLength = 0;
  int statusCode = 0;
  String detail;
  esp_http_client_handle_t client = openHttps(url, contentLength, statusCode, detail);
  if (client == nullptr) {
    error = withDetail(UI_TEXT("Güncelleme bilgisi alınamadı"), detail);
    return false;
  }
  if (statusCode != 200) {
    closeHttps(client);
    error = withDetail(UI_TEXT("Güncelleme bilgisi alınamadı"), String("HTTP ") + statusCode);
    return false;
  }

  String body;
  body.reserve(512);
  char chunk[256];
  int read = 0;
  while ((read = esp_http_client_read(client, chunk, sizeof(chunk))) > 0) {
    if (body.length() + static_cast<size_t>(read) > maximumManifestSize) {
      closeHttps(client);
      error = UI_TEXT("Güncelleme bilgisi geçersiz.");
      return false;
    }
    body.concat(chunk, static_cast<unsigned int>(read));
  }
  closeHttps(client);
  if (read < 0) {
    error = withDetail(UI_TEXT("Güncelleme bilgisi alınamadı"), "read");
    return false;
  }

  uint32_t version = 0;
  if (!jsonString(body, "version", manifest.version)
      || !parseVersion(manifest.version, version)
      || !jsonString(body, "file", manifest.file)
      || !isSafeFileName(manifest.file)
      || !jsonString(body, "sha256", manifest.sha256)
      || !isSha256Hex(manifest.sha256)
      || !jsonNumber(body, "size", manifest.size)
      || manifest.size == 0) {
    error = UI_TEXT("Güncelleme bilgisi geçersiz.");
    return false;
  }
  manifest.sha256.toLowerCase();
  jsonString(body, "notes", manifest.notes);
  if (manifest.notes.length() > maximumNotesLength) {
    manifest.notes = manifest.notes.substring(0, maximumNotesLength);
  }
  return true;
}

void OnlineUpdateService::runCheck()
{
  setState(OnlineUpdateState::checking, String());
  Manifest manifest;
  String error;
  const bool fetched = fetchManifest(manifest, error);
  lastCheckAt_ = millis();
  checkedOnce_ = true;
  lastCheckFailed_ = !fetched;
  if (!fetched) {
    Serial.printf("Online update check failed: %s\n", error.c_str());
    setState(OnlineUpdateState::failed, error);
    return;
  }

  uint32_t latest = 0;
  uint32_t current = 0;
  parseVersion(manifest.version, latest);
  parseVersion(String(AppConfig::firmwareVersion), current);
  {
    std::lock_guard<std::mutex> lock(mutex_);
    latest_ = manifest;
  }
  updateAvailable_ = latest > current;
  if (latest > current) {
    Serial.printf("Online update available: %s (running %s).\n", manifest.version.c_str(), AppConfig::firmwareVersion);
    setState(OnlineUpdateState::available, String());
  } else {
    setState(OnlineUpdateState::upToDate, String());
  }
}

void OnlineUpdateService::runInstall()
{
  Manifest manifest;
  {
    std::lock_guard<std::mutex> lock(mutex_);
    manifest = latest_;
  }
  progress_ = 0;
  setState(OnlineUpdateState::downloading, String());

  auto fail = [this](const String &message) {
    Serial.printf("Online update failed: %s\n", message.c_str());
    progress_ = 0;
    // The offer stays so the user can try again.
    setState(OnlineUpdateState::failed, message);
  };

  const esp_partition_t *target = esp_ota_get_next_update_partition(nullptr);
  if (target == nullptr || manifest.size > target->size) {
    fail(UI_TEXT("Firmware dosyası bu cihazın güncelleme alanına sığmıyor."));
    return;
  }
  if (heap_caps_get_free_size(MALLOC_CAP_INTERNAL) < AppConfig::OnlineUpdate::minimumFreeHeap) {
    fail(UI_TEXT("Yetersiz bellek; güncelleme sunucusuna bağlanılamadı."));
    return;
  }

  String url(AppConfig::OnlineUpdate::repositoryBaseUrl);
  url += manifest.file;
  int64_t contentLength = 0;
  int statusCode = 0;
  String detail;
  esp_http_client_handle_t client = openHttps(url, contentLength, statusCode, detail);
  if (client == nullptr) {
    fail(withDetail(UI_TEXT("Firmware indirilemedi"), detail));
    return;
  }
  if (statusCode != 200) {
    closeHttps(client);
    fail(withDetail(UI_TEXT("Firmware indirilemedi"), String("HTTP ") + statusCode));
    return;
  }
  if (contentLength >= 0 && static_cast<uint64_t>(contentLength) != manifest.size) {
    closeHttps(client);
    fail(UI_TEXT("İndirilen firmware güncelleme bilgisiyle uyuşmuyor; birkaç dakika sonra tekrar deneyin."));
    return;
  }
  if (!Update.begin(manifest.size, U_FLASH)) {
    closeHttps(client);
    fail(withDetail(UI_TEXT("OTA başlatılamadı"), Update.errorString()));
    return;
  }

  mbedtls_sha256_context sha;
  mbedtls_sha256_init(&sha);
  mbedtls_sha256_starts(&sha, 0);
  uint32_t received = 0;
  bool ok = true;
  while (received < manifest.size) {
    const size_t wanted = manifest.size - received < sizeof(buffer_)
      ? manifest.size - received
      : sizeof(buffer_);
    const int read = esp_http_client_read(
      client,
      reinterpret_cast<char *>(buffer_),
      static_cast<int>(wanted));
    if (read <= 0) {
      fail(withDetail(UI_TEXT("Firmware indirilemedi"), "connection"));
      ok = false;
      break;
    }
    mbedtls_sha256_update(&sha, buffer_, static_cast<size_t>(read));
    if (Update.write(buffer_, static_cast<size_t>(read)) != static_cast<size_t>(read)) {
      fail(withDetail(UI_TEXT("Firmware yazılamadı"), Update.errorString()));
      ok = false;
      break;
    }
    received += static_cast<uint32_t>(read);
    progress_ = static_cast<uint8_t>(static_cast<uint64_t>(received) * 100U / manifest.size);
  }
  closeHttps(client);

  uint8_t digest[32];
  mbedtls_sha256_finish(&sha, digest);
  mbedtls_sha256_free(&sha);
  if (!ok) {
    Update.abort();
    return;
  }

  char hex[65];
  for (size_t index = 0; index < sizeof(digest); ++index) {
    snprintf(hex + index * 2, 3, "%02x", digest[index]);
  }
  if (manifest.sha256 != hex) {
    Update.abort();
    fail(UI_TEXT("İndirilen firmware güncelleme bilgisiyle uyuşmuyor; birkaç dakika sonra tekrar deneyin."));
    return;
  }
  if (!Update.end()) {
    fail(withDetail(UI_TEXT("Firmware doğrulanamadı"), Update.errorString()));
    return;
  }

  progress_ = 100;
  Serial.printf("Online update %s written (%lu bytes).\n", manifest.version.c_str(), static_cast<unsigned long>(received));
  setState(OnlineUpdateState::installed, UI_TEXT("Güncelleme tamamlandı. Cihaz yeniden başlatılıyor."));
  installFinished_ = true;
}
