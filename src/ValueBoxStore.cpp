#include "ValueBoxStore.h"

#include <Arduino.h>
#include <cstddef>
#include <cstring>
#include <esp_rom_crc.h>
#include <memory>
#include <new>

#include "AppConfig.h"

namespace {

constexpr uint32_t sectorSize = 4096;
constexpr uint32_t recordSize = 16;
constexpr uint16_t slotsPerSector = sectorSize / recordSize;
constexpr uint8_t headerKind = 0xA1;
constexpr uint8_t valueKind = 0xB2;
// "VBX1": format of this log; a different value is never read back.
constexpr uint32_t headerMagic = 0x31584256UL;

static_assert(
  maxContentItems + 1 < slotsPerSector,
  "A sector must hold its header and a copy of every value");

} // namespace

bool ValueBoxStore::begin()
{
  static_assert(sizeof(Record) == recordSize, "Record layout changed");

  partition_ = esp_partition_find_first(
    ESP_PARTITION_TYPE_DATA,
    ESP_PARTITION_SUBTYPE_DATA_SPIFFS,
    AppConfig::ValueStore::partitionLabel);
  if (partition_ == nullptr || partition_->size < 2 * sectorSize) {
    Serial.println("ERROR: ValueBox store partition not found; values are not kept.");
    return false;
  }
  sectorCount_ = partition_->size / sectorSize;

  bool found = false;
  uint32_t newestSector = 0;
  uint32_t newestSequence = 0;
  for (uint32_t sector = 0; sector < sectorCount_; ++sector) {
    uint32_t sequence = 0;
    if (readHeader(sector, sequence)
        && (!found || sequence > newestSequence)) {
      found = true;
      newestSector = sector;
      newestSequence = sequence;
    }
  }

  ready_ = true;
  if (!found) {
    // Empty or foreign data: the first save erases sector 0.
    currentSector_ = sectorCount_ - 1;
    sequence_ = 0;
    nextSlot_ = slotsPerSector;
    Serial.printf(
      "ValueBox store ready: %lu KB, no stored values.\n",
      static_cast<unsigned long>(partition_->size / 1024));
    return true;
  }

  std::unique_ptr<Record[]> buffer(new (std::nothrow) Record[slotsPerSector]);
  if (!buffer) {
    ready_ = false;
    Serial.println("ERROR: ValueBox store could not allocate its read buffer.");
    return false;
  }

  // The newest sector starts with a copy of every value. If power failed
  // while that copy was being written, the rest is still in the sector
  // before it, so that one is read first.
  const uint32_t previousSector = (newestSector + sectorCount_ - 1) % sectorCount_;
  uint32_t previousSequence = 0;
  if (readHeader(previousSector, previousSequence)
      && previousSequence + 1 == newestSequence) {
    loadSector(previousSector, buffer.get(), nullptr);
  }
  if (!loadSector(newestSector, buffer.get(), &nextSlot_)) {
    ready_ = false;
    return false;
  }
  currentSector_ = newestSector;
  sequence_ = newestSequence;

  uint8_t count = 0;
  for (const Entry &stored : entries_) {
    count += stored.valid ? 1 : 0;
  }
  Serial.printf(
    "ValueBox store ready: %lu KB, %u stored value(s), sector %lu slot %u.\n",
    static_cast<unsigned long>(partition_->size / 1024),
    count,
    static_cast<unsigned long>(currentSector_),
    nextSlot_);
  return true;
}

bool ValueBoxStore::ready() const
{
  return ready_;
}

const ValueBoxStore::Entry &ValueBoxStore::entry(uint8_t index) const
{
  static const Entry empty;
  return index < maxContentItems ? entries_[index] : empty;
}

bool ValueBoxStore::save(uint8_t index, uint32_t identity, uint32_t bits)
{
  if (!ready_ || index >= maxContentItems) {
    return false;
  }
  if (nextSlot_ >= slotsPerSector && !startNextSector()) {
    return false;
  }
  if (!writeRecord(makeRecord(valueKind, index, identity, bits))) {
    return false;
  }
  entries_[index] = {true, identity, bits};
  return true;
}

void ValueBoxStore::forget(uint8_t index)
{
  if (index < maxContentItems) {
    entries_[index] = {};
  }
}

bool ValueBoxStore::clear()
{
  for (Entry &stored : entries_) {
    stored = {};
  }
  if (!ready_) {
    // Without the partition there is nothing to clear.
    return partition_ == nullptr;
  }
  return startNextSector() && startNextSector();
}

ValueBoxStore::Record ValueBoxStore::makeRecord(
  uint8_t kind,
  uint8_t index,
  uint32_t identity,
  uint32_t value)
{
  Record record = {};
  record.kind = kind;
  record.index = index;
  record.reserved = 0xFFFFU;
  record.identity = identity;
  record.value = value;
  record.crc = esp_rom_crc32_le(
    0,
    reinterpret_cast<const uint8_t *>(&record),
    offsetof(Record, crc));
  return record;
}

bool ValueBoxStore::validRecord(const Record &record)
{
  return record.crc == esp_rom_crc32_le(
    0,
    reinterpret_cast<const uint8_t *>(&record),
    offsetof(Record, crc));
}

bool ValueBoxStore::erasedRecord(const Record &record)
{
  const auto *bytes = reinterpret_cast<const uint8_t *>(&record);
  for (size_t index = 0; index < sizeof(record); ++index) {
    if (bytes[index] != 0xFF) {
      return false;
    }
  }
  return true;
}

bool ValueBoxStore::readHeader(uint32_t sector, uint32_t &sequence) const
{
  Record header = {};
  if (esp_partition_read(partition_, sector * sectorSize, &header, sizeof(header))
      != ESP_OK) {
    return false;
  }
  if (header.kind != headerKind
      || header.identity != headerMagic
      || !validRecord(header)) {
    return false;
  }
  sequence = header.value;
  return true;
}

bool ValueBoxStore::loadSector(
  uint32_t sector,
  Record *buffer,
  uint16_t *nextSlot)
{
  const esp_err_t error = esp_partition_read(
    partition_,
    sector * sectorSize,
    buffer,
    sectorSize);
  if (error != ESP_OK) {
    fail("read", error);
    return false;
  }

  // Records are written in slot order. A slot torn by a power loss fails
  // its checksum and is skipped; writing resumes after the last used slot
  // because flash cannot be programmed twice without an erase.
  uint16_t lastUsed = 0;
  for (uint16_t slot = 1; slot < slotsPerSector; ++slot) {
    const Record &record = buffer[slot];
    if (erasedRecord(record)) {
      continue;
    }
    lastUsed = slot;
    if (record.kind == valueKind
        && record.index < maxContentItems
        && validRecord(record)) {
      entries_[record.index] = {true, record.identity, record.value};
    }
  }
  if (nextSlot != nullptr) {
    *nextSlot = lastUsed + 1;
  }
  return true;
}

bool ValueBoxStore::startNextSector()
{
  const uint32_t sector = (currentSector_ + 1) % sectorCount_;
  const esp_err_t error = esp_partition_erase_range(
    partition_,
    sector * sectorSize,
    sectorSize);
  if (error != ESP_OK) {
    fail("erase", error);
    return false;
  }

  currentSector_ = sector;
  nextSlot_ = 0;
  ++sequence_;
  if (!writeRecord(makeRecord(headerKind, 0xFF, headerMagic, sequence_))) {
    return false;
  }
  for (uint8_t index = 0; index < maxContentItems; ++index) {
    const Entry &stored = entries_[index];
    if (stored.valid
        && !writeRecord(makeRecord(valueKind, index, stored.identity, stored.bits))) {
      return false;
    }
  }
  return true;
}

bool ValueBoxStore::writeRecord(const Record &record)
{
  const esp_err_t error = esp_partition_write(
    partition_,
    currentSector_ * sectorSize + static_cast<uint32_t>(nextSlot_) * recordSize,
    &record,
    sizeof(record));
  // A failed write may have programmed part of the slot; never reuse it.
  ++nextSlot_;
  if (error != ESP_OK) {
    fail("write", error);
    return false;
  }
  return true;
}

void ValueBoxStore::fail(const char *operation, esp_err_t error)
{
  ready_ = false;
  Serial.printf(
    "ERROR: ValueBox store %s failed: %s; values are no longer saved.\n",
    operation,
    esp_err_to_name(error));
}
