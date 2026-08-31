import fs from 'node:fs';
import path from 'node:path';
import zlib from 'node:zlib';

const projectRoot = process.cwd();
const portalPagePath = path.join(projectRoot, 'include', 'PortalPage.h');
const appConfigPath = path.join(projectRoot, 'include', 'AppConfig.h');
const outputPath = path.join(projectRoot, 'include', 'PortalCompressed.h');

const portalSource = fs.readFileSync(portalPagePath, 'utf8');
const appConfigSource = fs.readFileSync(appConfigPath, 'utf8');
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
const homeGzip = gzip(homeHtml);
const settingsGzip = gzip(settingsHtml);

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

} // namespace PortalCompressed
`;

fs.writeFileSync(outputPath, output, 'utf8');
console.log(`Generated PortalCompressed.h (${homeGzip.length} + ${settingsGzip.length} bytes).`);
