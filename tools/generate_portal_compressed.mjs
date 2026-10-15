import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';

const projectRoot = process.cwd();
const portalPagePath = path.join(projectRoot, 'include', 'PortalPage.h');
const appConfigPath = path.join(projectRoot, 'include', 'AppConfig.h');
const i18nPath = path.join(projectRoot, 'web', 'portal-i18n.js');
const outputPath = path.join(projectRoot, 'include', 'PortalCompressed.h');

const portalSource = fs.readFileSync(portalPagePath, 'utf8');
const appConfigSource = fs.readFileSync(appConfigPath, 'utf8');
const i18nSource = fs.readFileSync(i18nPath, 'utf8');
const versionMatch = appConfigSource.match(/#define\s+FIRMWARE_VERSION\s+"([^"]+)"/);
if (!versionMatch) {
  throw new Error('FIRMWARE_VERSION was not found in AppConfig.h.');
}
const firmwareVersion = versionMatch[1];

function declarationExpression(name, nextName) {
  const ending = nextName
    ? `\\r?\\n\\r?\\nconstexpr char ${nextName}Html`
    : '\\r?\\n\\r?\\n} // namespace PortalPage';
  const pattern = new RegExp(
    `constexpr char ${name}Html\\[\\] PROGMEM = ([\\s\\S]*?);${ending}`);
  const match = portalSource.match(pattern);
  if (!match) {
    throw new Error(`${name}Html declaration was not found.`);
  }
  return match[1];
}

function renderHtml(expression) {
  const tokens = expression.matchAll(/R"HTML\(([\s\S]*?)\)HTML"|(FIRMWARE_VERSION)/g);
  let html = '';
  for (const token of tokens) {
    html += token[1] !== undefined ? token[1] : firmwareVersion;
  }
  if (!html.startsWith('<!doctype html>') || !html.endsWith('</html>')) {
    throw new Error('Portal HTML could not be reconstructed.');
  }
  return html;
}

function gzip(html) {
  return zlib.gzipSync(Buffer.from(html, 'utf8'), {
    level: zlib.constants.Z_BEST_COMPRESSION,
    mtime: 0
  });
}

function validateInlineScripts(html, pageName) {
  const scripts = [...html.matchAll(/<script>([\s\S]*?)<\/script>/g)];
  for (const [index, script] of scripts.entries()) {
    try {
      new Function(script[1]);
    } catch (error) {
      throw new Error(`${pageName} inline script ${index + 1} is invalid: ${error.message}`);
    }
  }
}

// Translation table rows: [Turkish source, en, de, es, ar].
function translationRows() {
  const match = i18nSource.match(/const rows = (\[[\s\S]*?\n {2}\]);/);
  if (!match) {
    throw new Error('Translation rows were not found in portal-i18n.js.');
  }
  const rows = new Function(`return ${match[1]};`)();
  const keys = new Set();
  for (const [index, row] of rows.entries()) {
    if (!Array.isArray(row) || row.length !== 5
        || row.some(value => typeof value !== 'string' || value === '')) {
      throw new Error(`portal-i18n.js row ${index + 1} must contain 5 non-empty strings.`);
    }
    if (keys.has(row[0])) {
      throw new Error(`portal-i18n.js contains a duplicate key: ${row[0]}`);
    }
    keys.add(row[0]);
  }
  return keys;
}

function sourceFiles(directory, extensions) {
  return fs.readdirSync(directory, {withFileTypes: true}).flatMap(entry => {
    const entryPath = path.join(directory, entry.name);
    if (entry.isDirectory()) return sourceFiles(entryPath, extensions);
    return extensions.includes(path.extname(entry.name)) ? [entryPath] : [];
  });
}

// Every message the firmware sends to the portal is marked with UI_TEXT();
// without a translation the portal would show it in Turkish in every language.
function missingServerTranslations(keys) {
  const missing = new Set();
  const files = [
    ...sourceFiles(path.join(projectRoot, 'src'), ['.cpp', '.h']),
    ...sourceFiles(path.join(projectRoot, 'include'), ['.h']).filter(file => !file.endsWith('UiText.h'))
  ];
  for (const file of files) {
    const source = fs.readFileSync(file, 'utf8');
    for (const match of source.matchAll(/UI_TEXT\("((?:[^"\\]|\\.)*)"\)/g)) {
      const text = match[1].replace(/\\(.)/g, '$1');
      if (!keys.has(text)) missing.add(`${path.relative(projectRoot, file)}: ${text}`);
    }
  }
  return [...missing];
}

// Visible page text and the literal messages the page scripts pass to t(),
// setStatus(), showStatus() and setFirmwareProgress().
function missingPageTranslations(html, pageName, keys) {
  // Technical terms that read the same in every language, and placeholders
  // that the page script replaces with translated text after loading.
  const untranslated = new Set(['RGB', 'TextBox', 'ValueBox', '+ TextBox', '+ ValueBox',
    'Modbus RTU', 'STEP-RGB', 'STEP-RGB •', 'UTC', 'firmware.bin', 'Türkçe', 'English',
    'Deutsch', 'Español', 'العربية', 'admin', 'SSID', 'Baudrate', '5 bit', '6 bit', '7 bit',
    '8 bit', 'Signed integer (16-bit)', 'Float (IEEE-754)', '#FF0000', '°C, bar, kg',
    'CET-1CEST,M3.5.0,M10.5.0/3', '10 px/sn', 'Panel —', '0 / 10 alarm']);
  const decode = text => text.replace(/&lt;/g, '<').replace(/&gt;/g, '>').replace(/&amp;/g, '&');
  // I2C addresses with pin/part labels and plain bit/Hz options.
  const technical = /^(0x[0-9A-F]{2} — [A-Za-z0-9+ ,→-]+|\d+(\.\d+)? (bit|Hz)( — [\d.]+ µs)?)$/;
  const needsTranslation = text => /[A-Za-zÇĞİÖŞÜçğıöşü]{2}/.test(text)
    && !untranslated.has(text)
    && !technical.test(text);
  const missing = new Set();
  const markup = html.replace(/<script>[\s\S]*?<\/script>/g, '').replace(/<style>[\s\S]*?<\/style>/g, '');
  for (const match of markup.matchAll(/>([^<]+)</g)) {
    const text = decode(match[1].trim());
    if (text && needsTranslation(text) && !keys.has(text)) missing.add(text);
  }
  for (const match of markup.matchAll(/\s(?:placeholder|title|aria-label)="([^"]+)"/g)) {
    const text = decode(match[1]);
    if (needsTranslation(text) && !keys.has(text)) missing.add(text);
  }
  const scripts = [...html.matchAll(/<script>([\s\S]*?)<\/script>/g)].map(match => match[1]).join('\n');
  const literalCalls = /(?:\bt|setStatus|showStatus|setFirmwareProgress\([^,()]*,)\(?\s*'((?:[^'\\]|\\.)*)'/g;
  for (const match of scripts.matchAll(literalCalls)) {
    const text = match[1];
    if (needsTranslation(text) && !keys.has(text)) missing.add(text);
  }
  return [...missing].map(text => `${pageName}: ${text}`);
}

function formatBytes(bytes) {
  const lines = [];
  for (let index = 0; index < bytes.length; index += 16) {
    const values = [...bytes.subarray(index, index + 16)]
      .map(value => `0x${value.toString(16).padStart(2, '0')}`)
      .join(', ');
    lines.push(`  ${values}${index + 16 < bytes.length ? ',' : ''}`);
  }
  return lines.join('\n');
}

const homeHtml = renderHtml(declarationExpression('home', 'settings'));
const settingsHtml = renderHtml(declarationExpression('settings'));
validateInlineScripts(homeHtml, 'home');
validateInlineScripts(settingsHtml, 'settings');
try {
  new Function(i18nSource);
} catch (error) {
  throw new Error(`portal-i18n.js is invalid: ${error.message}`);
}
const translationKeys = translationRows();
const missingTranslations = [
  ...missingServerTranslations(translationKeys),
  ...missingPageTranslations(homeHtml, 'home', translationKeys),
  ...missingPageTranslations(settingsHtml, 'settings', translationKeys)
];
if (missingTranslations.length > 0) {
  throw new Error(`Missing translations in web/portal-i18n.js:\n  ${missingTranslations.join('\n  ')}`);
}
const homeGzip = gzip(homeHtml);
const settingsGzip = gzip(settingsHtml);
const i18nGzip = gzip(i18nSource);

const output = `#pragma once

#include <Arduino.h>

namespace PortalCompressed {

constexpr uint8_t homeGzip[] PROGMEM = {
${formatBytes(homeGzip)}
};
constexpr size_t homeGzipSize = sizeof(homeGzip);

constexpr uint8_t settingsGzip[] PROGMEM = {
${formatBytes(settingsGzip)}
};
constexpr size_t settingsGzipSize = sizeof(settingsGzip);

constexpr uint8_t i18nGzip[] PROGMEM = {
${formatBytes(i18nGzip)}
};
constexpr size_t i18nGzipSize = sizeof(i18nGzip);

} // namespace PortalCompressed
`;

fs.writeFileSync(outputPath, output, 'utf8');
console.log(`Generated PortalCompressed.h (${homeGzip.length} + ${settingsGzip.length} + ${i18nGzip.length} bytes).`);
