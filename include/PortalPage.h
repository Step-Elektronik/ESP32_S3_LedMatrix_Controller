#pragma once

#include <Arduino.h>

#include "AppConfig.h"

namespace PortalPage {

constexpr char homeHtml[] PROGMEM = R"HTML(<!doctype html>
<html lang="tr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
  <meta name="theme-color" content="#07111f">
  <title>STEP-RGB Panel Editörü</title>
  <style>
    :root {
      color-scheme: dark;
      font-family: Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      background: #07111f; color: #eaf2ff;
    }
    * { box-sizing: border-box; }
    html, body { min-height: 100%; margin: 0; }
    body {
      background:
        radial-gradient(circle at 12% 5%, rgba(36,119,255,.18), transparent 32rem),
        radial-gradient(circle at 92% 92%, rgba(0,220,180,.10), transparent 28rem),
        #07111f;
    }
    button, input, select { font: inherit; }
    button, a, input, select { -webkit-tap-highlight-color: transparent; }
    .shell { width: min(1180px,100%); margin: 0 auto; padding: 18px 18px 34px; }
    .topbar { display: flex; align-items: center; justify-content: space-between; gap: 16px; margin-bottom: 22px; }
    .brand { display: flex; align-items: center; gap: 12px; }
    .mark {
      width: 43px; height: 43px; display: grid; place-items: center; border-radius: 14px;
      color: white; font-size: 12px; font-weight: 900; letter-spacing: -.04em;
      background: linear-gradient(145deg,#2e8cff,#0068f0); box-shadow: 0 10px 28px rgba(0,105,240,.32);
    }
    h1 { margin: 0; font-size: clamp(21px,4vw,29px); }
    .subtitle { margin: 3px 0 0; color: #91a5c2; font-size: 13px; }
    .settings {
      width: 46px; height: 46px; display: grid; place-items: center; flex: 0 0 auto;
      color: #dce9ff; text-decoration: none; border: 1px solid rgba(255,255,255,.13);
      border-radius: 14px; background: rgba(255,255,255,.065);
    }
    .settings:active { transform: scale(.95); }
    .settings svg { width: 24px; height: 24px; fill: currentColor; }
    .layout { display: grid; grid-template-columns: minmax(0,1.55fr) minmax(300px,.75fr); gap: 18px; align-items: start; }
    .card {
      min-width: 0; border: 1px solid rgba(255,255,255,.10); border-radius: 20px;
      background: rgba(10,24,42,.82); box-shadow: 0 18px 48px rgba(0,0,0,.24); overflow: hidden;
    }
    .card-head { display: flex; align-items: center; justify-content: space-between; gap: 14px; padding: 17px 18px; border-bottom: 1px solid rgba(255,255,255,.08); }
    .card-title { margin: 0; font-size: 16px; }
    .card-note { margin: 3px 0 0; color: #8195b2; font-size: 12px; }
    .panel-pill { padding: 7px 10px; color: #aad0ff; white-space: nowrap; font-size: 12px; font-weight: 800; border-radius: 999px; background: rgba(38,125,255,.14); }
    .preview-body { padding: 18px; }
    .toolbar { display: flex; align-items: center; gap: 10px; margin-bottom: 14px; }
    .selection { min-width: 0; flex: 1; color: #aabbd2; font-size: 13px; }
    .selection strong { color: white; }
    .btn {
      min-height: 42px; padding: 9px 14px; color: white; cursor: pointer; font-weight: 800;
      border: 1px solid rgba(255,255,255,.12); border-radius: 12px; background: rgba(255,255,255,.07);
    }
    .btn.primary { border-color: #328aff; background: #1676ef; box-shadow: 0 8px 22px rgba(0,105,240,.22); }
    .btn.danger { color: #ffb5bd; border-color: rgba(255,80,100,.28); background: rgba(255,52,77,.10); }
    .btn:disabled { opacity: .42; cursor: not-allowed; box-shadow: none; }
    .stage-wrap {
      padding: 12px; border: 1px solid rgba(255,255,255,.10); border-radius: 16px;
      background: #03070c; box-shadow: inset 0 0 28px rgba(0,0,0,.85);
    }
    .stage {
      position: relative; width: 100%; overflow: hidden; user-select: none; touch-action: none;
      background-color: #050709;
      background-image: radial-gradient(circle,rgba(255,255,255,.13) 0 1px,transparent 1.25px);
      box-shadow: 0 0 25px rgba(15,70,130,.12);
    }
    .panel-grid, .item-layer { position: absolute; inset: 0; }
    .panel-grid { display: grid; z-index: 1; }
    .panel-cell { position: relative; min-width: 0; border: 1px solid rgba(123,160,201,.28); cursor: pointer; }
    .panel-cell.selected { border-color: #4da0ff; box-shadow: inset 0 0 0 1px #4da0ff, inset 0 0 25px rgba(29,126,255,.16); }
    .panel-label {
      position: absolute; left: 5px; top: 4px; z-index: 2; padding: 2px 5px; pointer-events: none;
      color: rgba(190,211,238,.7); font-size: clamp(8px,1.2vw,11px); font-weight: 800; border-radius: 5px; background: rgba(5,10,16,.7);
    }
    .panel-count { position: absolute; right: 5px; bottom: 4px; color: rgba(160,181,209,.58); font-size: 9px; pointer-events: none; }
    .item-layer { z-index: 3; pointer-events: none; }
    .display-item {
      position: absolute; max-width: none; color: #ff0000; cursor: grab; pointer-events: auto; touch-action: none;
      min-width: 1px; min-height: 1px; outline: 1px dashed rgba(255,255,255,.46); border-radius: 2px;
    }
    .display-item canvas { display: block; pointer-events: none; image-rendering: pixelated; filter: drop-shadow(0 0 2px currentColor); }
    .display-item.selected { outline-style: solid; outline-color: #fff; box-shadow: 0 0 0 2px rgba(58,145,255,.68); }
    .display-item.dragging { cursor: grabbing; opacity: .82; }
    .move-badge {
      position: absolute; right: -8px; top: -10px; min-width: 16px; height: 16px; display: grid; place-items: center;
      color: #06101d; font: 900 10px/1 sans-serif; border-radius: 999px; background: #e7f2ff; box-shadow: 0 2px 7px #000;
    }
    .stage-help { display: flex; justify-content: space-between; gap: 12px; margin-top: 11px; color: #778da9; font-size: 12px; }
    .stage-help strong { color: #aec7e7; }
    .empty { padding: 38px 22px; color: #8296b2; text-align: center; }
    .editor { padding: 18px; }
    .editor-placeholder { padding: 36px 12px; color: #8296b2; text-align: center; line-height: 1.55; }
    .field { margin-bottom: 15px; }
    .field label, .field-title { display: block; margin: 0 0 7px; color: #aebed4; font-size: 12px; font-weight: 800; }
    input[type="text"], input[type="number"], select {
      width: 100%; min-height: 43px; padding: 10px 12px; color: #edf5ff;
      border: 1px solid rgba(255,255,255,.12); border-radius: 11px; outline: none; background: rgba(255,255,255,.055);
    }
    select option, select optgroup { color: #102139; background: #f4f7fb; }
    input:focus, select:focus { border-color: #4497ff; box-shadow: 0 0 0 3px rgba(35,129,255,.14); }
    input:disabled, select:disabled { opacity: .4; }
    .segments { display: grid; grid-template-columns: repeat(4,1fr); gap: 7px; }
    .segments.value-source-segments { grid-template-columns: repeat(2,1fr); }
    .segment { min-height: 40px; padding: 8px; color: #9cb0ca; cursor: pointer; font-weight: 800; border: 1px solid rgba(255,255,255,.10); border-radius: 10px; background: rgba(255,255,255,.045); }
    .segment.active { color: white; border-color: #3d91ff; background: rgba(41,127,255,.20); }
    .color-row { display: grid; grid-template-columns: 54px 1fr; align-items: center; gap: 10px; }
    input[type="color"] { width: 54px; height: 43px; padding: 3px; cursor: pointer; border: 1px solid rgba(255,255,255,.14); border-radius: 11px; background: transparent; }
    .color-code { color: #aebed4; font: 700 13px/1 monospace; }
    .switch-row { display: flex; align-items: center; justify-content: space-between; gap: 12px; }
    .switch { position: relative; width: 47px; height: 27px; flex: 0 0 auto; }
    .switch input { position: absolute; opacity: 0; }
    .slider { position: absolute; inset: 0; cursor: pointer; border-radius: 999px; background: #34465d; transition: .16s; }
    .slider::after { content: ""; position: absolute; width: 21px; height: 21px; left: 3px; top: 3px; border-radius: 50%; background: white; transition: .16s; }
    .switch input:checked + .slider { background: #1676ef; }
    .switch input:checked + .slider::after { transform: translateX(20px); }
    .cycle-options { margin: -4px 0 15px; padding: 13px; border: 1px solid rgba(65,145,255,.22); border-radius: 13px; background: rgba(35,112,225,.07); }
    .cycle-options[hidden] { display: none; }
    .cycle-note { margin: 0 0 11px; color: #8fa7c5; font-size: 11px; line-height: 1.45; }
    .cycle-choices { display: grid; grid-template-columns: repeat(2,1fr); gap: 8px; margin-bottom: 12px; }
    .cycle-choice { position: relative; }
    .cycle-choice input { position: absolute; opacity: 0; }
    .cycle-choice span { display: grid; min-height: 38px; place-items: center; padding: 7px; color: #9db0c9; cursor: pointer; font-size: 12px; font-weight: 800; border: 1px solid rgba(255,255,255,.10); border-radius: 10px; background: rgba(255,255,255,.04); }
    .cycle-choice input:checked + span { color: #fff; border-color: #438fff; background: rgba(42,126,246,.22); }
    .cycle-badge { position: absolute; left: -8px; top: -10px; min-width: 16px; height: 16px; display: grid; place-items: center; color: #06101d; font: 900 11px/1 sans-serif; border-radius: 999px; background: #62e2c1; box-shadow: 0 2px 7px #000; }
    .two-col { display: grid; grid-template-columns: 1fr 1fr; gap: 10px; }
    .range-head { display: flex; justify-content: space-between; align-items: center; margin-bottom: 6px; }
    input[type="range"] { width: 100%; accent-color: #2587ff; }
    .coords { display: grid; grid-template-columns: 1fr 1fr; gap: 8px; }
    .coord { padding: 9px 10px; color: #98abc3; font-size: 12px; text-align: center; border: 1px solid rgba(255,255,255,.08); border-radius: 10px; background: rgba(0,0,0,.16); }
    .coord b { color: #edf5ff; }
    .actions { display: flex; justify-content: space-between; gap: 10px; padding-top: 4px; }
    .savebar { display: flex; justify-content: flex-end; align-items: center; gap: 12px; margin-top: 16px; }
    .status { min-width: 0; flex: 1; color: #91a5c2; font-size: 12px; }
    .status.ok { color: #70e0b6; } .status.error { color: #ff9aa6; }
    footer { display: flex; justify-content: center; margin-top: 24px; }
    .version { padding: 7px 11px; color: rgba(221,234,255,.60); font-size: 11px; font-weight: 700; letter-spacing: .045em; border: 1px solid rgba(255,255,255,.08); border-radius: 999px; }
    @media (max-width: 850px) { .layout { grid-template-columns: 1fr; } }
    @media (max-width: 540px) {
      .shell { padding: 14px 12px 28px; } .subtitle { display: none; }
      .card-head, .preview-body, .editor { padding: 14px; }
      .toolbar { align-items: stretch; flex-wrap: wrap; } .toolbar .selection { flex-basis: 100%; } .toolbar .btn { flex: 1; padding-inline: 11px; }
      .stage-wrap { padding: 7px; } .stage-help { display: block; line-height: 1.55; }
      .two-col { grid-template-columns: 1fr; } .savebar { position: sticky; bottom: 8px; z-index: 20; padding: 8px; border-radius: 14px; background: rgba(7,17,31,.93); }
    }
    @media (prefers-reduced-motion: reduce) { *,*::before,*::after { transition: none !important; } }
  </style>
</head>
<body>
  <div class="shell">
    <header class="topbar">
      <div class="brand">
        <div class="mark">RGB</div>
        <div><h1>Panel İçerik Editörü</h1><p class="subtitle">Metin, saat, tarih ve sıcaklığı panel üzerinde konumlandırın</p></div>
      </div>
      <a class="settings" href="/settings" aria-label="Ayarlar" title="Ayarlar">
        <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M19.14 12.94c.04-.31.06-.63.06-.94s-.02-.63-.07-.94l2.03-1.58a.49.49 0 0 0 .12-.61l-1.92-3.32a.49.49 0 0 0-.59-.22l-2.39.96a7.08 7.08 0 0 0-1.62-.94L14.4 2.8a.48.48 0 0 0-.48-.4h-3.84a.48.48 0 0 0-.48.4l-.36 2.54c-.58.24-1.12.56-1.62.94l-2.39-.96a.48.48 0 0 0-.59.22L2.72 8.87a.48.48 0 0 0 .12.61l2.03 1.58c-.05.31-.08.64-.08.94s.03.63.08.94l-2.03 1.58a.49.49 0 0 0-.12.61l1.92 3.32c.12.22.38.31.59.22l2.39-.96c.5.39 1.04.71 1.62.95l.36 2.54c.04.23.24.4.48.4h3.84c.24 0 .44-.17.48-.4l.36-2.54c.58-.24 1.12-.56 1.62-.95l2.39.96c.21.09.47 0 .59-.22l1.92-3.32a.49.49 0 0 0-.12-.61l-2.02-1.58ZM12 15.5A3.5 3.5 0 1 1 12 8a3.5 3.5 0 0 1 0 7.5Z"/></svg>
      </a>
    </header>

    <main class="layout">
      <section class="card" aria-labelledby="preview-title">
        <div class="card-head">
          <div><h2 class="card-title" id="preview-title">RGB Panel Önizleme</h2><p class="card-note">Bir panel seçin, öğe ekleyin ve sürükleyerek taşıyın</p></div>
          <span class="panel-pill" id="panelSize">—</span>
        </div>
        <div class="preview-body">
          <div class="toolbar">
            <div class="selection" id="panelSelection">Panel bilgisi yükleniyor…</div>
            <button class="btn" id="addTextBox" type="button" disabled>+ TextBox</button>
            <button class="btn" id="addValueBox" type="button" disabled>+ ValueBox</button>
          </div>
          <div class="stage-wrap">
            <div class="stage" id="stage" aria-label="Sürüklenebilir panel önizlemesi">
              <div class="panel-grid" id="panelGrid"></div>
              <div class="item-layer" id="itemLayer"></div>
            </div>
          </div>
          <div class="stage-help"><span><strong>İpucu:</strong> Öğeyi parmağınızla veya fareyle sürükleyin.</span><span id="logicalSize">—</span></div>
          <div class="savebar">
            <span class="status" id="status">İçerik yükleniyor…</span>
            <button class="btn primary" id="saveAll" type="button" disabled>Panele Uygula</button>
          </div>
        </div>
      </section>

      <aside class="card" aria-labelledby="editor-title">
        <div class="card-head"><div><h2 class="card-title" id="editor-title">Kutu Ayarları</h2><p class="card-note">Seçilen öğenin görünümü, kaynağı ve hareketi</p></div></div>
        <div class="editor-placeholder" id="editorPlaceholder">Düzenlemek için panelden bir öğe seçin veya yeni TextBox/ValueBox ekleyin.</div>
        <div class="editor" id="editor" hidden>
          <div id="textBoxOptions">
          <div class="field">
            <span class="field-title">Veri türü</span>
            <div class="segments" id="typeSegments">
              <button class="segment" type="button" data-type="0">Yazı</button>
              <button class="segment" type="button" data-type="1">Saat</button>
              <button class="segment" type="button" data-type="2">Tarih</button>
              <button class="segment" type="button" data-type="3">Sıcaklık</button>
            </div>
          </div>
          <div class="field switch-row">
            <div><span class="field-title">İçerik döngüsü</span><div class="card-note">Birden fazla bilgiyi aynı TextBox'ta sırayla gösterir</div></div>
            <label class="switch"><input id="cycleEnabled" type="checkbox"><span class="slider"></span></label>
          </div>
          <div class="cycle-options" id="cycleOptions" hidden>
            <p class="cycle-note">İşaretlenen bilgiler Yazı → Saat → Tarih → Sıcaklık sırasıyla gösterilir. Kayar yazı açıksa içerik tam turunu tamamladıktan sonra değişir.</p>
            <div class="cycle-choices">
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="0"><span>Yazı</span></label>
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="1"><span>Saat</span></label>
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="2"><span>Tarih</span></label>
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="3"><span>Sıcaklık</span></label>
            </div>
            <div class="field" style="margin-bottom:0"><label for="cycleInterval">Değişim sıklığı (saniye)</label><input id="cycleInterval" type="number" min="1" max="3600" value="5" inputmode="numeric"></div>
          </div>
          <div class="field">
            <label for="textValue">Yazdırılacak metin</label>
            <input id="textValue" type="text" maxlength="40" autocomplete="off" placeholder="Metninizi yazın">
          </div>
          <div class="field" id="temperatureFormatOptions" hidden>
            <label for="temperatureFormat">Sıcaklık gösterimi</label>
            <select id="temperatureFormat">
              <option value="0">Float — bir ondalık (23.5°C)</option>
              <option value="1">Integer — tam sayı (23.5 → 24°C)</option>
            </select>
            <div class="card-note">Integer seçiminde en yakın tam sayıya yuvarlanır; 0.5 ve üzeri bir üst değere çıkar.</div>
          </div>
          </div>
          <div id="valueBoxOptions" hidden>
            <div class="field">
              <label for="initialValue">Başlangıç değeri</label>
              <input id="initialValue" type="number" step="1" value="0" inputmode="decimal">
              <div class="card-note">Cihaz her açıldığında ValueBox bu değerden başlar.</div>
            </div>
            <div class="field">
              <span class="field-title">Veri kaynağı</span>
              <div class="segments value-source-segments">
                <button class="segment" type="button" data-value-source="0">Modbus RTU</button>
                <button class="segment" type="button" data-value-source="1">Dijital giriş</button>
              </div>
            </div>
            <div id="modbusOptions">
              <p class="cycle-note">Slave ID: 1 • 9600 baud • 8N1 • Holding register. IEEE-754 float seçilen adresten başlayarak iki register kullanır (yüksek word önce).</p>
              <div class="two-col">
                <div class="field"><label for="registerAddress">Register adresi</label><input id="registerAddress" type="number" min="0" max="65535" value="0" inputmode="numeric"></div>
                <div class="field"><label for="valueType">Değer tipi</label><select id="valueType"><option value="0">Signed integer (16-bit)</option><option value="1">Float (IEEE-754)</option></select></div>
              </div>
            </div>
            <div id="digitalInputOptions" hidden>
              <div class="field"><label for="counterDirection">Sayaç yönü</label><select id="counterDirection"><option value="0">Yukarı sayıcı</option><option value="1">Aşağı sayıcı</option></select></div>
              <p class="cycle-note">IO21 dahili pull-down ile LOW seviyesinde bekler. Debounce sonrasındaki LOW → HIGH yükselen kenarında değer artırılır veya azaltılır. Negatif değerler gösterilebilir.</p>
            </div>
          </div>
          <div class="field">
            <label for="itemFont">Yazı tipi</label>
            <select id="itemFont"></select>
            <div class="card-note">TextBox ve ValueBox için cihazdaki tüm Adafruit GFX fontları kullanılabilir.</div>
          </div>
          <div class="field">
            <label for="itemRotation">Kutu yönü</label>
            <select id="itemRotation"><option value="0">0°</option><option value="1">90° saat yönü</option><option value="2">180°</option><option value="3">270° saat yönü</option></select>
          </div>
          <div class="field">
            <span class="field-title">Renk</span>
            <div class="color-row"><input id="itemColor" type="color" value="#ff0000"><span class="color-code" id="colorCode">#FF0000</span></div>
          </div>
          <div class="field switch-row">
            <div><span class="field-title">Kayar yazı</span><div class="card-note">Ekran yenilenmesini engellemeden hareket eder</div></div>
            <label class="switch"><input id="scrollEnabled" type="checkbox"><span class="slider"></span></label>
          </div>
          <div class="two-col">
            <div class="field"><label for="scrollDirection">Yön</label><select id="scrollDirection"><option value="0">Sola</option><option value="1">Sağa</option><option value="2">Yukarı</option><option value="3">Aşağı</option></select></div>
            <div class="field">
              <div class="range-head"><label for="scrollSpeed">Hız</label><span class="color-code" id="speedValue">10 px/sn</span></div>
              <input id="scrollSpeed" type="range" min="1" max="100" value="10">
            </div>
          </div>
          <div class="field">
            <span class="field-title">Konum</span>
            <div class="coords"><div class="coord">X: <b id="coordX">0</b></div><div class="coord">Y: <b id="coordY">0</b></div></div>
          </div>
          <div class="actions"><button class="btn danger" id="deleteItem" type="button">Kutuyu Sil</button><span class="panel-pill" id="itemPanel">Panel —</span></div>
        </div>
      </aside>
    </main>
    <footer><span class="version">STEP-RGB • Yazılım v)HTML"
FIRMWARE_VERSION
R"HTML(</span></footer>
  </div>

  <script>
    (() => {
      'use strict';
      const PW = 32, PH = 16;
      const state = { rows:1, columns:1, maxPerPanel:2, fonts:[], defaultFont:2, fontData:new Map(), fontLoads:new Map(), items:[], selectedPanel:0, selectedId:null, nextId:1, drag:null, temperatureValue:null };
      const $ = id => document.getElementById(id);
      const stage = $('stage'), grid = $('panelGrid'), layer = $('itemLayer');
      const status = $('status'), addTextButton = $('addTextBox'), addValueButton = $('addValueBox'), saveButton = $('saveAll');
      const typeNames = ['Yazı','Saat','Tarih','Sıcaklık'];
      const boxNames = ['TextBox','ValueBox'];
      const directionArrows = ['←','→','↑','↓'];

      function setStatus(message, kind='') {
        status.textContent = message;
        status.className = 'status' + (kind ? ' ' + kind : '');
      }
      function width() { return state.columns * PW; }
      function height() { return state.rows * PH; }
      function panelCount(index) { return state.items.filter(item => item.panel === index).length; }
      function selectedItem() { return state.items.find(item => item.id === state.selectedId) || null; }
      function toHex(value) { return Number(value).toString(16).padStart(2,'0'); }
      function itemColor(item) { return '#' + toHex(item.red) + toHex(item.green) + toHex(item.blue); }
      function cycleTypes(item) { return [0,1,2,3].filter(type => (item.cycleMask & (1<<type)) !== 0); }
      function activeType(item) {
        if (!item.cycleEnabled) return item.type;
        const types=cycleTypes(item); if (!types.length) return item.type;
        const interval=Math.max(1,Number(item.cycleInterval)||5)*1000;
        const elapsed=Math.max(0,Date.now()-(item._cycleStartedAt||Date.now()));
        return types[Math.floor(elapsed/interval)%types.length];
      }
      function textForType(item,type) {
        const now = new Date(), two = value => String(value).padStart(2,'0');
        if (type === 1) return two(now.getHours()) + ':' + two(now.getMinutes());
        if (type === 2) return two(now.getDate()) + '/' + two(now.getMonth()+1) + '/' + two(now.getFullYear()%100);
        if (type === 3) {
          if (!Number.isFinite(state.temperatureValue)) return 'TMP?';
          const value=Number(state.temperatureValue);
          const rounded=value<0?-Math.round(-value):Math.round(value);
          return (item.temperatureFormat===1?String(rounded):value.toFixed(1))+'°C';
        }
        return item.text || 'YAZI';
      }
      function formatInitialValue(item) {
        const value=Number(item.initialValue);
        if(!Number.isFinite(value)) return '0';
        return item.valueSource===0&&item.valueType===1 ? value.toFixed(1) : String(Math.trunc(value));
      }
      function liveText(item) {
        return item.kind===1 ? (item.currentValue||formatInitialValue(item)) : textForType(item,activeType(item));
      }
      function fontName(item) { return state.fonts[item.font] || state.fonts[state.defaultFont] || 'Apple6x10'; }
      const turkishGlyphCodes = {'Ç':0x7F,'Ğ':0x80,'İ':0x81,'Ö':0x82,'Ş':0x83,'Ü':0x84,'ç':0x85,'ğ':0x86,'ı':0x87,'ö':0x88,'ş':0x89,'ü':0x8A,'°':0x8B};
      function encodedFontCodes(item,text) {
        const codes=[];
        for(const character of text) {
          const code=character.codePointAt(0);
          if(code<0x80) codes.push(code);
          else if(character==='℃') codes.push(0x8B,0x43);
          else if(turkishGlyphCodes[character]!==undefined) codes.push(turkishGlyphCodes[character]);
          else codes.push('?'.charCodeAt(0));
        }
        return codes;
      }
      function parseFontData(buffer,fontId) {
        const view=new DataView(buffer);
        if(buffer.byteLength<14 || view.getUint8(0)!==71 || view.getUint8(1)!==70 || view.getUint8(2)!==88 || view.getUint8(3)!==49 || view.getUint8(4)!==fontId) throw new Error('Yazı tipi önizleme verisi geçersiz.');
        const first=view.getUint8(5), last=view.getUint8(6), yAdvance=view.getUint8(7);
        const glyphCount=view.getUint16(8,true), bitmapSize=view.getUint32(10,true);
        const bitmapStart=14+glyphCount*8;
        if(glyphCount!==last-first+1 || bitmapStart+bitmapSize!==buffer.byteLength) throw new Error('Yazı tipi önizleme verisi eksik.');
        const glyphs=[];
        for(let index=0;index<glyphCount;index++) {
          const offset=14+index*8;
          glyphs.push({bitmapOffset:view.getUint16(offset,true),width:view.getUint8(offset+2),height:view.getUint8(offset+3),xAdvance:view.getUint8(offset+4),xOffset:view.getInt8(offset+5),yOffset:view.getInt8(offset+6)});
        }
        return {first,last,yAdvance,glyphs,bitmap:new Uint8Array(buffer,bitmapStart,bitmapSize)};
      }
      async function loadFontData(fontId) {
        if(state.fontData.has(fontId)) return state.fontData.get(fontId);
        if(state.fontLoads.has(fontId)) return state.fontLoads.get(fontId);
        const pending=(async()=>{
          const response=await fetch('/api/font?id='+fontId,{cache:'no-store'});
          if(!response.ok) throw new Error('Yazı tipi önizleme verisi alınamadı.');
          const data=parseFontData(await response.arrayBuffer(),fontId);
          state.fontData.set(fontId,data);
          return data;
        })();
        state.fontLoads.set(fontId,pending);
        try { return await pending; }
        finally { state.fontLoads.delete(fontId); }
      }
      function previewText(item,type) {
        return item.kind===1 ? liveText(item) : textForType(item,type);
      }
      function gfxLayout(item,type) {
        const font=state.fontData.get(item.font);
        const codes=encodedFontCodes(item,previewText(item,type));
        let cursorX=0, cursorY=0, minX=32767, minY=32767, maxX=-1, maxY=-1;
        if(font) for(const code of codes) {
          if(code===10){cursorX=0;cursorY+=font.yAdvance;continue;}
          if(code===13 || code<font.first || code>font.last) continue;
          const glyph=font.glyphs[code-font.first];
          const x1=cursorX+glyph.xOffset, y1=cursorY+glyph.yOffset;
          const x2=x1+glyph.width-1, y2=y1+glyph.height-1;
          if(x1<minX) minX=x1; if(y1<minY) minY=y1;
          if(x2>maxX) maxX=x2; if(y2>maxY) maxY=y2;
          cursorX+=glyph.xAdvance;
        }
        const valid=maxX>=minX&&maxY>=minY;
        return {font,codes,x:valid?minX:0,y:valid?minY:0,width:valid?maxX-minX+1:0,height:valid?maxY-minY+1:0};
      }
      function previewTextSize(item,type) {
        const layout=gfxLayout(item,type);
        return item.rotation===1||item.rotation===3
          ? {width:layout.height,height:layout.width}
          : {width:layout.width,height:layout.height};
      }
      function alignedOrigin(item) {
        if(!item.cycleEnabled) return {x:item.x,y:item.y};
        const reference=previewTextSize(item,item.type);
        const active=previewTextSize(item,activeType(item));
        return {x:item.x+Math.trunc((reference.width-active.width)/2),y:item.y+Math.trunc((reference.height-active.height)/2)};
      }
      function positionPreviewNode(node,item,sx,sy) {
        const origin=alignedOrigin(item);
        node.style.left=(origin.x*sx)+'px';
        node.style.top=(origin.y*sy)+'px';
      }
      function drawPreviewCanvas(canvas,item,sx,sy) {
        const type=activeType(item), layout=gfxLayout(item,type);
        const rotated=item.rotation===1||item.rotation===3;
        const visualWidth=rotated?layout.height:layout.width;
        const visualHeight=rotated?layout.width:layout.height;
        canvas.width=Math.max(1,visualWidth); canvas.height=Math.max(1,visualHeight);
        canvas.style.width=(Math.max(1,visualWidth)*sx)+'px';
        canvas.style.height=(Math.max(1,visualHeight)*sy)+'px';
        if(!layout.font || !layout.width || !layout.height) return;
        const context=canvas.getContext('2d'); context.fillStyle=itemColor(item);
        const hideColon=item.kind===0&&type===1&&(Math.floor(Date.now()/1000)&1)!==0;
        let cursorX=0, cursorY=0;
        for(const code of layout.codes) {
          if(code===10){cursorX=0;cursorY+=layout.font.yAdvance;continue;}
          if(code===13 || code<layout.font.first || code>layout.font.last) continue;
          const glyph=layout.font.glyphs[code-layout.font.first];
          let bitmapOffset=glyph.bitmapOffset, bits=0, bitIndex=0;
          if(!(hideColon&&code===58)) for(let sourceY=0;sourceY<glyph.height;sourceY++) for(let sourceX=0;sourceX<glyph.width;sourceX++) {
            if((bitIndex++&7)===0) bits=layout.font.bitmap[bitmapOffset++];
            if((bits&0x80)!==0) {
              const localX=cursorX+glyph.xOffset+sourceX-layout.x;
              const localY=cursorY+glyph.yOffset+sourceY-layout.y;
              let drawX=localX, drawY=localY;
              if(item.rotation===1){drawX=layout.height-1-localY;drawY=localX;}
              else if(item.rotation===2){drawX=layout.width-1-localX;drawY=layout.height-1-localY;}
              else if(item.rotation===3){drawX=localY;drawY=layout.width-1-localX;}
              context.fillRect(drawX,drawY,1,1);
            }
            bits=(bits<<1)&0xFF;
          }
          cursorX+=glyph.xAdvance;
        }
      }
      function renderPreviewNode(node,item,sx,sy) {
        positionPreviewNode(node,item,sx,sy);
        drawPreviewCanvas(node.querySelector('canvas'),item,sx,sy);
      }
      function utf8Length(value) { return encodeURIComponent(value).replace(/%[0-9A-F]{2}|./gi,'x').length; }

      function selectPanel(index) {
        state.selectedPanel = Math.max(0, Math.min(state.rows * state.columns - 1, index));
        renderStage();
      }
      function selectItem(id) {
        state.selectedId = id;
        const item = selectedItem();
        if (item) state.selectedPanel = item.panel;
        renderStage();
        syncEditor();
      }

      function renderStage() {
        const total = state.rows * state.columns;
        stage.style.aspectRatio = width() + ' / ' + height();
        grid.style.gridTemplateColumns = `repeat(${state.columns},1fr)`;
        grid.style.gridTemplateRows = `repeat(${state.rows},1fr)`;
        grid.replaceChildren();
        for (let index=0; index<total; index++) {
          const cell = document.createElement('div');
          cell.className = 'panel-cell' + (index === state.selectedPanel ? ' selected' : '');
          cell.dataset.panel = index;
          const label = document.createElement('span');
          label.className = 'panel-label'; label.textContent = 'P' + (index+1);
          const count = document.createElement('span');
          count.className = 'panel-count'; count.textContent = panelCount(index) + '/2';
          cell.append(label,count);
          cell.addEventListener('click', () => selectPanel(index));
          grid.appendChild(cell);
        }

        layer.replaceChildren();
        const rect = stage.getBoundingClientRect();
        const sx = rect.width / width(), sy = rect.height / height();
        state.items.forEach(item => {
          const node = document.createElement('div');
          node.className = 'display-item'
            + (item.id === state.selectedId ? ' selected' : '');
          node.dataset.id = item.id;
          node.style.color = itemColor(item);
          const canvas=document.createElement('canvas');
          canvas.setAttribute('aria-label',liveText(item));
          node.appendChild(canvas);
          renderPreviewNode(node,item,sx,sy);
          node.title = (item.kind===1
            ? boxNames[item.kind]+' • '+(item.valueSource===0?'Modbus':'Dijital giriş')
            : item.cycleEnabled
            ? 'Döngü: '+cycleTypes(item).map(type=>typeNames[type]).join(' → ')
            : typeNames[item.type]) + ' • '+fontName(item)+' • P' + (item.panel+1);
          if (item.kind===0 && item.cycleEnabled) {
            const badge = document.createElement('span');
            badge.className = 'cycle-badge'; badge.textContent = '↻';
            node.appendChild(badge);
          }
          if (item.scrolling) {
            const badge = document.createElement('span');
            badge.className = 'move-badge'; badge.textContent = directionArrows[item.direction];
            node.appendChild(badge);
          }
          node.addEventListener('pointerdown', event => startDrag(event,item,node));
          node.addEventListener('click', event => { event.stopPropagation(); selectItem(item.id); });
          layer.appendChild(node);
        });
        const count = panelCount(state.selectedPanel);
        $('panelSelection').innerHTML = '<strong>Panel ' + (state.selectedPanel+1) + '</strong> seçili • ' + count + '/2 öğe';
        addTextButton.disabled = count >= state.maxPerPanel;
        addValueButton.disabled = count >= state.maxPerPanel;
        $('panelSize').textContent = state.columns + ' × ' + state.rows + ' panel';
        $('logicalSize').textContent = width() + ' × ' + height() + ' piksel';
      }

      function startDrag(event,item,node) {
        event.preventDefault(); event.stopPropagation();
        state.selectedId=item.id; state.selectedPanel=item.panel;
        layer.querySelectorAll('.display-item').forEach(value => value.classList.toggle('selected',value===node));
        grid.querySelectorAll('.panel-cell').forEach(value => value.classList.toggle('selected',Number(value.dataset.panel)===item.panel));
        syncEditor();
        const rect = stage.getBoundingClientRect(), sx = rect.width/width(), sy = rect.height/height();
        const origin=alignedOrigin(item);
        state.drag = { item, node, pointerId:event.pointerId,
          offsetX:(event.clientX-rect.left)/sx-origin.x, offsetY:(event.clientY-rect.top)/sy-origin.y,
          alignX:origin.x-item.x, alignY:origin.y-item.y };
        node.classList.add('dragging'); node.setPointerCapture(event.pointerId);
      }
      function moveDrag(event) {
        if (!state.drag || event.pointerId !== state.drag.pointerId) return;
        const rect = stage.getBoundingClientRect(), sx = rect.width/width(), sy = rect.height/height();
        const x = Math.max(0,Math.min(width()-1,Math.round((event.clientX-rect.left)/sx-state.drag.offsetX-state.drag.alignX)));
        const y = Math.max(0,Math.min(height()-1,Math.round((event.clientY-rect.top)/sy-state.drag.offsetY-state.drag.alignY)));
        const target = Math.floor(y/PH)*state.columns + Math.floor(x/PW);
        if (target !== state.drag.item.panel && panelCount(target) >= state.maxPerPanel) {
          setStatus('Hedef panelde toplam en fazla 2 TextBox/ValueBox olabilir.','error'); return;
        }
        state.drag.item.x=x; state.drag.item.y=y; state.drag.item.panel=target; state.selectedPanel=target;
        positionPreviewNode(state.drag.node,state.drag.item,sx,sy);
        $('coordX').textContent=x; $('coordY').textContent=y; $('itemPanel').textContent='Panel '+(target+1);
      }
      function stopDrag(event) {
        if (!state.drag || event.pointerId !== state.drag.pointerId) return;
        state.drag.node.classList.remove('dragging'); state.drag=null;
        renderStage(); syncEditor(); setStatus('Konum değişti. Kaydetmeyi unutmayın.');
      }
      stage.addEventListener('pointermove',moveDrag);
      stage.addEventListener('pointerup',stopDrag);
      stage.addEventListener('pointercancel',stopDrag);
      stage.addEventListener('click',event => {
        if (event.target.closest('.display-item')) return;
        const rect=stage.getBoundingClientRect();
        const x=Math.max(0,Math.min(width()-1,Math.floor((event.clientX-rect.left)*width()/rect.width)));
        const y=Math.max(0,Math.min(height()-1,Math.floor((event.clientY-rect.top)*height()/rect.height)));
        selectPanel(Math.floor(y/PH)*state.columns+Math.floor(x/PW));
      });

      function nextRegisterAddress() {
        const occupied=new Set();
        state.items.filter(item=>item.kind===1&&item.valueSource===0).forEach(item=>{
          occupied.add(Number(item.registerAddress));
          if(item.valueType===1) occupied.add(Number(item.registerAddress)+1);
        });
        for(let address=0;address<65535;address++) if(!occupied.has(address)) return address;
        return 0;
      }
      function addItem(kind) {
        if (panelCount(state.selectedPanel) >= state.maxPerPanel) return;
        const col=state.selectedPanel%state.columns, row=Math.floor(state.selectedPanel/state.columns);
        const slot=panelCount(state.selectedPanel);
        const item={ id:state.nextId++, kind, panel:state.selectedPanel, type:0, rotation:0,
          x:col*PW+1, y:row*PH+1+slot*8, red:255, green:0, blue:0,
          scrolling:false, direction:0, speed:10, cycleEnabled:false, cycleMask:1,
          cycleInterval:5, _cycleStartedAt:Date.now(), text:'YAZI', initialValue:0,
          valueSource:0, registerAddress:nextRegisterAddress(), valueType:0,
          counterDirection:0, temperatureFormat:0, font:state.defaultFont, currentValue:'0' };
        state.items.push(item); state.selectedId=item.id;
        renderStage(); syncEditor(); setStatus('Yeni '+boxNames[kind]+' eklendi.');
      }
      function deleteItem() {
        const item=selectedItem(); if (!item) return;
        const name=boxNames[item.kind];
        state.items=state.items.filter(value => value.id !== item.id); state.selectedId=null;
        renderStage(); syncEditor(); setStatus(name+' silindi. Değişikliği uygulayabilirsiniz.');
      }

      function syncEditor() {
        const item=selectedItem(), editor=$('editor'), placeholder=$('editorPlaceholder');
        editor.hidden=!item; placeholder.hidden=!!item;
        if (!item) return;
        const isValue=item.kind===1;
        $('editor-title').textContent=boxNames[item.kind]+' Ayarları';
        $('textBoxOptions').hidden=isValue;
        $('valueBoxOptions').hidden=!isValue;
        $('deleteItem').textContent=boxNames[item.kind]+'\'u Sil';
        document.querySelectorAll('[data-type]').forEach(button => {
          const active=Number(button.dataset.type)===item.type;
          button.classList.toggle('active',active); button.setAttribute('aria-pressed',String(active));
          button.disabled=item.cycleEnabled;
        });
        const cycleUsesText=item.cycleEnabled && (item.cycleMask&1)!==0;
        $('textValue').value=item.text; $('textValue').disabled=item.cycleEnabled?!cycleUsesText:item.type!==0;
        $('cycleEnabled').checked=item.cycleEnabled;
        $('cycleOptions').hidden=!item.cycleEnabled;
        document.querySelectorAll('[data-cycle-type]').forEach(input => { input.checked=(item.cycleMask&(1<<Number(input.dataset.cycleType)))!==0; });
        $('cycleInterval').value=item.cycleInterval;
        const usesTemperature=item.cycleEnabled?(item.cycleMask&8)!==0:item.type===3;
        $('temperatureFormatOptions').hidden=!usesTemperature;
        $('temperatureFormat').value=item.temperatureFormat;
        $('initialValue').value=item.initialValue;
        $('initialValue').step=item.valueSource===0&&item.valueType===1?'any':'1';
        document.querySelectorAll('[data-value-source]').forEach(button => {
          const active=Number(button.dataset.valueSource)===item.valueSource;
          button.classList.toggle('active',active); button.setAttribute('aria-pressed',String(active));
        });
        $('modbusOptions').hidden=item.valueSource!==0;
        $('digitalInputOptions').hidden=item.valueSource!==1;
        $('registerAddress').value=item.registerAddress;
        $('valueType').value=item.valueType;
        $('counterDirection').value=item.counterDirection;
        $('itemFont').value=item.font;
        $('itemRotation').value=item.rotation;
        $('itemColor').value=itemColor(item); $('colorCode').textContent=itemColor(item).toUpperCase();
        $('scrollEnabled').checked=item.scrolling;
        $('scrollDirection').value=item.direction; $('scrollDirection').disabled=!item.scrolling;
        $('scrollSpeed').value=item.speed; $('scrollSpeed').disabled=!item.scrolling;
        $('speedValue').textContent=item.speed+' px/sn';
        $('coordX').textContent=item.x; $('coordY').textContent=item.y;
        $('itemPanel').textContent='Panel '+(item.panel+1);
      }

      document.querySelectorAll('[data-type]').forEach(button => button.addEventListener('click',() => {
        const item=selectedItem(); if (!item) return;
        item.type=Number(button.dataset.type); item.cycleMask=1<<item.type; item._cycleStartedAt=Date.now(); syncEditor(); renderStage(); setStatus('Veri türü değişti.');
      }));
      $('cycleEnabled').addEventListener('change',event => {
        const item=selectedItem(); if(!item) return;
        item.cycleEnabled=event.target.checked;
        if(!item.cycleMask) item.cycleMask=1<<item.type;
        item._cycleStartedAt=Date.now(); syncEditor(); renderStage();
        setStatus(item.cycleEnabled?'İçerik döngüsü açıldı.':'İçerik döngüsü kapatıldı.');
      });
      document.querySelectorAll('[data-cycle-type]').forEach(input => input.addEventListener('change',event => {
        const item=selectedItem(); if(!item) return;
        const bit=1<<Number(event.target.dataset.cycleType);
        const nextMask=event.target.checked?item.cycleMask|bit:item.cycleMask&~bit;
        if(!nextMask){ event.target.checked=true; setStatus('Döngü için en az bir bilgi seçilmelidir.','error'); return; }
        item.cycleMask=nextMask; item._cycleStartedAt=Date.now(); syncEditor(); renderStage();
      }));
      $('cycleInterval').addEventListener('input',event => { const item=selectedItem(); if(item){ item.cycleInterval=Number(event.target.value); item._cycleStartedAt=Date.now(); renderStage(); } });
      $('textValue').addEventListener('input',event => { const item=selectedItem(); if(item){ item.text=event.target.value; renderStage(); } });
      $('temperatureFormat').addEventListener('change',event => {
        const item=selectedItem(); if(!item||item.kind!==0) return;
        item.temperatureFormat=Number(event.target.value)===1?1:0;
        renderStage(); setStatus('Sıcaklık gösterim tipi değişti.');
      });
      document.querySelectorAll('[data-value-source]').forEach(button => button.addEventListener('click',()=>{
        const item=selectedItem(); if(!item||item.kind!==1) return;
        item.valueSource=Number(button.dataset.valueSource); item.currentValue=formatInitialValue(item);
        syncEditor(); renderStage(); setStatus('ValueBox veri kaynağı değişti.');
      }));
      $('initialValue').addEventListener('input',event => {
        const item=selectedItem(); if(!item||item.kind!==1) return;
        item.initialValue=event.target.value; item.currentValue=formatInitialValue(item); renderStage();
      });
      $('registerAddress').addEventListener('input',event => { const item=selectedItem(); if(item&&item.kind===1) item.registerAddress=Number(event.target.value); });
      $('valueType').addEventListener('change',event => {
        const item=selectedItem(); if(!item||item.kind!==1) return;
        item.valueType=Number(event.target.value); item.currentValue=formatInitialValue(item); syncEditor(); renderStage();
      });
      $('counterDirection').addEventListener('change',event => { const item=selectedItem(); if(item&&item.kind===1) item.counterDirection=Number(event.target.value); });
      $('itemFont').addEventListener('change',async event => {
        const item=selectedItem(); if(!item) return;
        const previousFont=item.font;
        item.font=Number(event.target.value);
        setStatus('Yazı tipi önizlemesi hazırlanıyor…');
        try { await loadFontData(item.font); renderStage(); setStatus('Yazı tipi değişti.','ok'); }
        catch(error) { item.font=previousFont; syncEditor(); renderStage(); setStatus(error.message,'error'); }
      });
      $('itemRotation').addEventListener('change',event => { const item=selectedItem(); if(item){ item.rotation=Number(event.target.value); renderStage(); setStatus('Kutu yönü değişti.'); } });
      $('itemColor').addEventListener('input',event => {
        const item=selectedItem(); if(!item) return;
        const hex=event.target.value; item.red=parseInt(hex.slice(1,3),16); item.green=parseInt(hex.slice(3,5),16); item.blue=parseInt(hex.slice(5,7),16);
        $('colorCode').textContent=hex.toUpperCase(); renderStage();
      });
      $('scrollEnabled').addEventListener('change',event => { const item=selectedItem(); if(item){ item.scrolling=event.target.checked; syncEditor(); renderStage(); } });
      $('scrollDirection').addEventListener('change',event => { const item=selectedItem(); if(item){ item.direction=Number(event.target.value); renderStage(); } });
      $('scrollSpeed').addEventListener('input',event => { const item=selectedItem(); if(item){ item.speed=Number(event.target.value); $('speedValue').textContent=item.speed+' px/sn'; } });
      addTextButton.addEventListener('click',()=>addItem(0));
      addValueButton.addEventListener('click',()=>addItem(1));
      $('deleteItem').addEventListener('click',deleteItem);

      async function request(url,options) {
        const response=await fetch(url,{cache:'no-store',...options});
        const data=await response.json().catch(()=>({ok:false,error:'Geçersiz cihaz yanıtı.'}));
        if(!response.ok || data.ok===false) throw new Error(data.error||'İşlem başarısız.');
        return data;
      }
      async function loadContent() {
        try {
          const data=await request('/api/content');
          state.rows=data.panel.rows; state.columns=data.panel.columns; state.maxPerPanel=data.panel.maxPerPanel;
          state.fonts=Array.isArray(data.fonts)?data.fonts:[];
          state.defaultFont=Number.isInteger(Number(data.defaultFont))?Number(data.defaultFont):2;
          const fontSelect=$('itemFont'); fontSelect.replaceChildren();
          state.fonts.forEach((name,id)=>{
            const option=document.createElement('option'); option.value=String(id); option.textContent=name; fontSelect.appendChild(option);
          });
          state.items=data.items.map(item=>({...item,kind:Number(item.kind)||0,rotation:item.rotation||0,
            cycleEnabled:Boolean(item.cycleEnabled),cycleMask:Number(item.cycleMask)||(1<<item.type),
            cycleInterval:Number(item.cycleInterval)||5,initialValue:Number(item.initialValue)||0,
            valueSource:Number(item.valueSource)||0,registerAddress:Number(item.registerAddress)||0,
            valueType:Number(item.valueType)||0,counterDirection:Number(item.counterDirection)||0,
            temperatureFormat:Number(item.temperatureFormat)===1?1:0,
            font:Number.isInteger(Number(item.font))?Number(item.font):state.defaultFont,
            currentValue:String(item.currentValue||''),_cycleStartedAt:Date.now(),id:state.nextId++}));
          const requiredFonts=new Set([state.defaultFont,...state.items.map(item=>item.font)]);
          for(const fontId of requiredFonts) await loadFontData(fontId);
          state.selectedPanel=0; state.selectedId=state.items.length?state.items[0].id:null;
          renderStage(); syncEditor(); saveButton.disabled=false;
          setStatus('Panel içeriği hazır.','ok');
        } catch(error) { setStatus(error.message,'error'); }
      }
      async function loadTemperature() {
        try {
          const data=await request('/api/temperature');
          state.temperatureValue=data.ready && Number.isFinite(data.temperature)
            ? Number(data.temperature)
            : null;
        } catch(error) { state.temperatureValue=null; }
      }
      async function saveContent() {
        const registerRanges=[];
        for (const item of state.items) {
          if(!Number.isInteger(item.font)||item.font<0||item.font>=state.fonts.length){ selectItem(item.id); setStatus('Yazı tipi seçimi geçersiz.','error'); return; }
          if(item.kind===0){
            const textRequired=(!item.cycleEnabled&&item.type===0)||(item.cycleEnabled&&(item.cycleMask&1)!==0);
            if (textRequired && !item.text.trim()) { selectItem(item.id); setStatus('Yazı içeriği boş bırakılamaz.','error'); return; }
            if (utf8Length(item.text)>47) { selectItem(item.id); setStatus('Metin en fazla 47 bayt olabilir.','error'); return; }
            if (item.cycleEnabled && !item.cycleMask) { selectItem(item.id); setStatus('Döngü için en az bir bilgi seçilmelidir.','error'); return; }
            if (item.cycleEnabled && (!Number.isInteger(item.cycleInterval)||item.cycleInterval<1||item.cycleInterval>3600)) { selectItem(item.id); setStatus('Değişim sıklığı 1-3600 saniye olmalıdır.','error'); return; }
            continue;
          }

          const initial=Number(item.initialValue);
          if(!Number.isFinite(initial)){ selectItem(item.id); setStatus('ValueBox başlangıç değeri geçersiz.','error'); return; }
          if(item.valueSource===1){
            if(!Number.isInteger(initial)||initial<-2147483648||initial>2147483647){ selectItem(item.id); setStatus('Dijital sayaç başlangıç değeri 32-bit bir integer olmalıdır.','error'); return; }
            continue;
          }
          if(item.valueType===0&&(!Number.isInteger(initial)||initial<-32768||initial>32767)){ selectItem(item.id); setStatus('Modbus integer başlangıç değeri -32768 ile 32767 arasında olmalıdır.','error'); return; }
          if(!Number.isInteger(item.registerAddress)||item.registerAddress<0||item.registerAddress>65535||(item.valueType===1&&item.registerAddress===65535)){ selectItem(item.id); setStatus('ValueBox register adresi geçersiz.','error'); return; }
          const range={start:item.registerAddress,end:item.registerAddress+(item.valueType===1?1:0)};
          if(registerRanges.some(other=>range.start<=other.end&&other.start<=range.end)){ selectItem(item.id); setStatus('Modbus ValueBox register adresleri çakışamaz.','error'); return; }
          registerRanges.push(range);
        }
        const records=state.items.map(item => [item.panel,item.type,item.rotation,item.x,item.y,
          itemColor(item).slice(1).toUpperCase(),item.scrolling?1:0,item.direction,item.speed,
          encodeURIComponent(item.text||''),item.cycleEnabled?1:0,item.cycleMask,item.cycleInterval,
          item.kind,Number(item.initialValue)||0,item.valueSource,item.registerAddress,item.valueType,
          item.counterDirection,item.temperatureFormat,item.font].join('|')).join('\n');
        saveButton.disabled=true; setStatus('Panele uygulanıyor…');
        try { await request('/api/content',{method:'POST',headers:{'Content-Type':'text/plain;charset=utf-8'},body:records}); setStatus('İçerik panele uygulandı ve flash belleğe kaydedildi.','ok'); }
        catch(error) { setStatus(error.message,'error'); }
        finally { saveButton.disabled=false; }
      }
      saveButton.addEventListener('click',saveContent);
      window.addEventListener('resize',renderStage);
      setInterval(() => {
        if(state.drag) return;
        const rect=stage.getBoundingClientRect(), sx=rect.width/width(), sy=rect.height/height();
        document.querySelectorAll('.display-item').forEach(node => {
          const item=state.items.find(value=>String(value.id)===node.dataset.id);
          if(!item) return;
          const canvas=node.querySelector('canvas');
          canvas.setAttribute('aria-label',liveText(item));
          renderPreviewNode(node,item,sx,sy);
        });
      },1000);
      loadContent();
      loadTemperature();
      setInterval(loadTemperature, 2000);
    })();
  </script>
</body>
</html>)HTML";

constexpr char settingsHtml[] PROGMEM = R"HTML(<!doctype html>
<html lang="tr">
<head>
  <meta charset="utf-8">
  <meta name="viewport" content="width=device-width,initial-scale=1,viewport-fit=cover">
  <meta name="theme-color" content="#07111f">
  <title>STEP-RGB Ayarlar</title>
  <style>
    :root {
      color-scheme: dark;
      font-family: Inter, ui-sans-serif, system-ui, -apple-system, BlinkMacSystemFont, "Segoe UI", sans-serif;
      background: #07111f;
      color: #eaf2ff;
    }
    * { box-sizing: border-box; }
    html, body { min-height: 100%; margin: 0; }
    body {
      min-height: 100vh;
      background:
        radial-gradient(circle at 12% 5%, rgba(36, 119, 255, .18), transparent 32rem),
        radial-gradient(circle at 92% 92%, rgba(0, 220, 180, .10), transparent 28rem),
        #07111f;
    }
    button, input, select { font: inherit; }
    .shell { width: min(920px, 100%); margin: 0 auto; padding: 20px 18px 42px; }
    .topbar { display: flex; align-items: center; gap: 14px; margin-bottom: 22px; }
    .back {
      width: 44px; height: 44px; display: grid; place-items: center;
      color: #dce9ff; text-decoration: none; font-size: 25px;
      border: 1px solid rgba(255,255,255,.13); border-radius: 14px;
      background: rgba(255,255,255,.065);
    }
    h1 { margin: 0; font-size: clamp(22px, 4vw, 30px); }
    .subtitle { margin: 4px 0 0; color: #91a5c2; font-size: 13px; }
    .tabs { display: grid; grid-template-columns: repeat(3, 1fr); gap: 10px; margin-bottom: 18px; }
    .tab {
      min-height: 58px; padding: 10px 12px; color: #b9c9df; cursor: pointer;
      border: 1px solid rgba(255,255,255,.10); border-radius: 15px;
      background: rgba(255,255,255,.055); font-weight: 700;
    }
    .tab.active { color: white; border-color: #4592ff; background: rgba(44,126,255,.20); }
    .panel {
      display: none; padding: clamp(18px, 4vw, 28px);
      border: 1px solid rgba(255,255,255,.10); border-radius: 20px;
      background: rgba(9,24,42,.88); box-shadow: 0 22px 60px rgba(0,0,0,.28);
    }
    .panel.active { display: block; }
    h2 { margin: 0 0 6px; font-size: 21px; }
    .note { margin: 0 0 22px; color: #91a5c2; font-size: 13px; line-height: 1.5; }
    .sta-warning {
      grid-column: 1 / -1;
      display: flex;
      align-items: flex-start;
      gap: 10px;
      margin: 0;
      padding: 12px 14px;
      color: #ffd995;
      font-size: 12px;
      line-height: 1.45;
      border: 1px solid rgba(255,180,61,.32);
      border-radius: 12px;
      background: rgba(255,153,31,.10);
    }
    .sta-warning strong { color: #ffe7b8; }
    .grid { display: grid; grid-template-columns: repeat(2, minmax(0,1fr)); gap: 16px; }
    .field { display: grid; gap: 7px; }
    .field.full { grid-column: 1 / -1; }
    label { color: #b8c9df; font-size: 13px; font-weight: 700; }
    input, select {
      width: 100%; height: 46px; padding: 0 13px; color: #f1f6ff;
      border: 1px solid rgba(255,255,255,.13); border-radius: 12px;
      background: #0b1a2d; outline: none;
    }
    input:focus, select:focus { border-color: #4b98ff; box-shadow: 0 0 0 3px rgba(55,138,255,.15); }
    input[readonly] { color: #7f94af; background: #091523; }
    input[type="range"] { height: 38px; padding: 0; accent-color: #3488ff; }
    .range-row { display: grid; grid-template-columns: 1fr 50px; align-items: center; gap: 12px; }
    output { text-align: center; color: #fff; font-weight: 800; }
    fieldset { grid-column: 1 / -1; display: grid; grid-template-columns: repeat(2,minmax(0,1fr)); gap: 16px;
      margin: 0; padding: 18px; border: 1px solid rgba(255,255,255,.10); border-radius: 15px; }
    legend { padding: 0 8px; color: #d7e6fa; font-weight: 800; }
    .sleep-toggle {
      grid-column: 1 / -1; display: flex; align-items: center; gap: 11px;
      min-height: 46px; padding: 11px 13px; cursor: pointer;
      border: 1px solid rgba(80,151,255,.25); border-radius: 12px;
      background: rgba(38,115,226,.09);
    }
    .sleep-toggle input { width: 20px; height: 20px; padding: 0; margin: 0; accent-color: #3488ff; }
    .sleep-hint {
      grid-column: 1 / -1; margin: 0; color: #91a5c2;
      font-size: 12px; line-height: 1.5;
    }
    .wifi-connection {
      display: grid; grid-template-columns: auto minmax(0,1fr) auto; align-items: center; gap: 12px;
      margin: 15px 0 18px; padding: 14px 16px; border: 1px solid rgba(255,255,255,.11);
      border-radius: 14px; background: #0a1829;
    }
    .wifi-dot { width: 11px; height: 11px; border-radius: 50%; background: #71839b; box-shadow: 0 0 0 5px rgba(113,131,155,.12); }
    .wifi-copy { display: grid; gap: 3px; min-width: 0; }
    .wifi-copy strong { color: #e7f0fc; font-size: 14px; }
    .wifi-copy span { overflow-wrap: anywhere; color: #91a7c3; font-size: 12px; }
    .wifi-signal { display: flex; align-items: center; gap: 8px; margin-top: 5px; }
    .wifi-signal[hidden] { display: none; }
    .wifi-signal-track {
      flex: 0 0 86px; height: 7px; overflow: hidden; border-radius: 999px;
      background: rgba(255,255,255,.12);
    }
    .wifi-signal-track span {
      display: block; width: 0; height: 100%; border-radius: inherit;
      background: #32ca70; transition: width .2s ease, background-color .2s ease;
    }
    .wifi-copy .wifi-signal-text { color: #b8c9dd; white-space: nowrap; }
    .wifi-ip { padding: 7px 10px; color: #a9f5c7; border-radius: 9px; background: rgba(33,173,91,.13); font-size: 13px; }
    .wifi-connection[data-state="connecting"] .wifi-dot { background: #3c91ff; box-shadow: 0 0 0 5px rgba(60,145,255,.15); }
    .wifi-connection[data-state="paused"] .wifi-dot { background: #f0ad3d; box-shadow: 0 0 0 5px rgba(240,173,61,.15); }
    .wifi-connection[data-state="connected"] { border-color: rgba(50,202,112,.34); }
    .wifi-connection[data-state="connected"] .wifi-dot { background: #32ca70; box-shadow: 0 0 0 5px rgba(50,202,112,.15); }
    .wifi-connection[data-state="failed"] { border-color: rgba(239,76,91,.34); }
    .wifi-connection[data-state="failed"] .wifi-dot { background: #ef4c5b; box-shadow: 0 0 0 5px rgba(239,76,91,.15); }
    .actions { display: flex; flex-wrap: wrap; justify-content: flex-end; gap: 10px; margin-top: 22px; }
    .button {
      min-height: 44px; padding: 10px 18px; color: white; cursor: pointer;
      border: 0; border-radius: 12px; background: #247cf0; font-weight: 800;
    }
    .button.secondary { background: #21344d; color: #d9e7fa; }
    .button:disabled { opacity: .45; cursor: not-allowed; }
    .alarm-summary {
      display: flex; align-items: center; justify-content: space-between; gap: 12px;
      margin-bottom: 14px; color: #91a5c2; font-size: 13px;
    }
    .alarm-summary strong { color: #dce9fa; }
    .alarm-list { display: grid; gap: 13px; }
    .alarm-empty {
      margin: 0; padding: 22px; color: #8297b4; text-align: center;
      border: 1px dashed rgba(255,255,255,.14); border-radius: 14px;
    }
    .alarm-card {
      padding: 15px; border: 1px solid rgba(255,255,255,.11);
      border-radius: 15px; background: #0a1829;
    }
    .alarm-card-head { display: flex; align-items: center; justify-content: space-between; gap: 12px; margin-bottom: 13px; }
    .alarm-title { color: #e7f0fc; font-size: 14px; }
    .alarm-remove {
      min-height: 34px; padding: 6px 11px; color: #ffb5bc; cursor: pointer;
      border: 1px solid rgba(239,76,91,.28); border-radius: 9px;
      background: rgba(239,76,91,.10); font-weight: 800;
    }
    .alarm-fields { display: grid; grid-template-columns: repeat(3,minmax(0,1fr)); gap: 12px; }
    .alarm-custom-days {
      grid-column: 1 / -1; display: grid; gap: 10px; padding: 13px;
      border: 1px solid rgba(65,145,255,.22); border-radius: 12px;
      background: rgba(35,112,225,.07);
    }
    .alarm-custom-days[hidden] { display: none; }
    .alarm-day-grid { display: grid; grid-template-columns: repeat(4,minmax(0,1fr)); gap: 8px; }
    .alarm-day-choice { position: relative; }
    .alarm-day-choice input { position: absolute; opacity: 0; pointer-events: none; }
    .alarm-day-choice span {
      display: grid; min-height: 38px; place-items: center; padding: 7px;
      color: #9db0c9; cursor: pointer; font-size: 12px; font-weight: 800;
      border: 1px solid rgba(255,255,255,.10); border-radius: 10px;
      background: rgba(255,255,255,.04);
    }
    .alarm-day-choice input:checked + span {
      color: #fff; border-color: #438fff; background: rgba(42,126,246,.22);
    }
    .tests { display: grid; grid-template-columns: repeat(3,1fr); gap: 10px; }
    .test-red { background: #da3042; }
    .test-green { background: #159b57; }
    .test-blue { background: #2575df; }
    .connection-grid { display: grid; grid-template-columns: repeat(4, minmax(0,1fr)); gap: 10px; }
    .connection-card {
      min-width: 0; padding: 10px 8px 9px; color: #9fb2cc; cursor: pointer;
      border: 1px solid rgba(255,255,255,.11); border-radius: 14px;
      background: #0a1829; transition: border-color .15s, background .15s, color .15s;
    }
    .connection-card:hover { border-color: rgba(93,159,255,.65); color: #dce9fb; }
    .connection-card.selected {
      color: #fff; border-color: #398cff; background: rgba(42,126,243,.19);
      box-shadow: 0 0 0 2px rgba(57,140,255,.12);
    }
    .connection-card svg { width: 100%; height: 70px; display: block; }
    .connection-card .route {
      fill: none; stroke: currentColor; stroke-width: 9; stroke-linecap: round; stroke-linejoin: round;
    }
    .connection-card .tip { fill: currentColor; stroke: none; }
    .connection-name { display: block; min-height: 30px; margin-top: 4px; font-size: 11px; font-weight: 800; line-height: 1.25; }
    .firmware-card {
      display: grid; gap: 16px; padding: 18px;
      border: 1px solid rgba(255,255,255,.11); border-radius: 15px;
      background: #0a1829;
    }
    .firmware-file {
      height: auto; min-height: 50px; padding: 6px; cursor: pointer;
    }
    .firmware-file::file-selector-button {
      min-height: 36px; margin-right: 12px; padding: 7px 13px; color: #e7f0fc;
      border: 0; border-radius: 9px; background: #21344d; cursor: pointer;
      font-weight: 800;
    }
    .firmware-meta { margin: 0; color: #91a5c2; font-size: 12px; line-height: 1.5; overflow-wrap: anywhere; }
    .firmware-progress { display: grid; grid-template-columns: minmax(0,1fr) 52px; align-items: center; gap: 10px; }
    .firmware-progress[hidden] { display: none; }
    .firmware-progress-track {
      height: 12px; overflow: hidden; border-radius: 999px;
      background: rgba(255,255,255,.12);
    }
    .firmware-progress-bar {
      display: block; width: 0; height: 100%; border-radius: inherit;
      background: linear-gradient(90deg,#247cf0,#33d0a1); transition: width .12s linear;
    }
    .firmware-progress-value { text-align: right; }
    .firmware-progress-text { grid-column: 1 / -1; margin: 0; color: #b8c9df; font-size: 12px; }
    .status {
      position: fixed; left: 50%; bottom: max(18px, env(safe-area-inset-bottom)); z-index: 10;
      max-width: calc(100% - 36px); padding: 11px 16px; color: white;
      border: 1px solid rgba(255,255,255,.14); border-radius: 999px;
      background: #172b46; box-shadow: 0 14px 35px rgba(0,0,0,.38);
      opacity: 0; transform: translate(-50%, 20px); pointer-events: none;
      transition: opacity .2s, transform .2s;
    }
    .status.show { opacity: 1; transform: translate(-50%, 0); }
    .status.error { background: #8f2734; }
    footer { margin-top: 18px; text-align: center; color: #7388a4; font-size: 12px; }
    @media (max-width: 650px) {
      .shell { padding: 16px 12px 34px; }
      .tabs { grid-template-columns: 1fr; }
      .tab { min-height: 48px; }
      .grid, fieldset { grid-template-columns: 1fr; }
      .field.full, fieldset { grid-column: auto; }
      .connection-grid { grid-template-columns: repeat(2, minmax(0,1fr)); }
      .actions .button { flex: 1; }
      .wifi-connection { grid-template-columns: auto minmax(0,1fr); }
      .wifi-ip { grid-column: 2; justify-self: start; }
      .alarm-fields { grid-template-columns: 1fr; }
      .alarm-day-grid { grid-template-columns: repeat(2,minmax(0,1fr)); }
    }
  </style>
</head>
<body>
  <div class="shell">
    <div class="topbar">
      <a class="back" href="/" aria-label="Geri">&#8249;</a>
      <div><h1>Ayarlar</h1><p class="subtitle">STEP-RGB panel kontrolü</p></div>
    </div>

    <nav class="tabs" aria-label="Ayar grupları">
      <button class="tab active" data-target="panel">Panel Ayarları</button>
      <button class="tab" data-target="clock">Saat ve Tarih Ayarları</button>
      <button class="tab" data-target="alarm">Alarm Ayarları</button>
      <button class="tab" data-target="communication">Haberleşme Ayarları</button>
      <button class="tab" data-target="wifi">Wi-Fi Ayarları</button>
      <button class="tab" data-target="firmware">Yazılım Güncelleme</button>
    </nav>

    <section class="panel active" id="panel-section">
      <h2>Panel Ayarları</h2>
      <p class="note">Tarama ve yerleşim değişiklikleri kaydedildikten sonra cihaz otomatik yeniden başlar.</p>
      <form id="panel-form">
        <div class="grid">
          <div class="field">
            <label for="scan">Panel yenileme / tarama oranı</label>
            <select id="scan" name="scan"><option value="4">1/4</option><option value="8">1/8</option></select>
          </div>
          <div class="field">
            <label for="count">Panel sayısı</label>
            <input id="count" readonly value="1">
          </div>
          <div class="field">
            <label for="rows">Satır sayısı</label>
            <input id="rows" name="rows" type="number" min="1" max="4" required>
          </div>
          <div class="field">
            <label for="columns">Sütun sayısı</label>
            <input id="columns" name="columns" type="number" min="1" max="4" required>
          </div>
          <div class="field full">
            <label for="brightness">Parlaklık</label>
            <div class="range-row"><input id="brightness" name="brightness" type="range" min="0" max="255"><output id="brightness-value">64</output></div>
          </div>
          <div class="field full">
            <label>Panel bağlantı şekli</label>
            <input id="connectionStyle" name="connectionStyle" type="hidden" value="2">
            <div class="connection-grid" role="radiogroup" aria-label="Panel bağlantı şekli">
              <button class="connection-card" type="button" data-connection="1" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M12 14H62C79 14 79 36 62 36H31C16 36 16 56 31 56H78"/><path class="tip" d="M76 47L92 56 76 66Z"/></svg>
                <span class="connection-name">Sol üstten aşağı</span>
              </button>
              <button class="connection-card" type="button" data-connection="2" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M88 14H38C21 14 21 36 38 36H69C84 36 84 56 69 56H22"/><path class="tip" d="M24 47L8 56 24 66Z"/></svg>
                <span class="connection-name">Sağ üstten aşağı</span>
              </button>
              <button class="connection-card" type="button" data-connection="3" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M12 56H62C79 56 79 34 62 34H31C16 34 16 14 31 14H78"/><path class="tip" d="M76 4L92 14 76 23Z"/></svg>
                <span class="connection-name">Sol alttan yukarı</span>
              </button>
              <button class="connection-card" type="button" data-connection="4" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M88 56H38C21 56 21 34 38 34H69C84 34 84 14 69 14H22"/><path class="tip" d="M24 4L8 14 24 23Z"/></svg>
                <span class="connection-name">Sağ alttan yukarı</span>
              </button>
              <button class="connection-card" type="button" data-connection="5" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M18 10V42C18 59 40 59 40 42V27C40 13 62 13 62 27V53"/><path class="tip" d="M52 51L62 67 72 51Z"/></svg>
                <span class="connection-name">Sol üstten aşağı · ZZ</span>
              </button>
              <button class="connection-card" type="button" data-connection="6" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M82 10V42C82 59 60 59 60 42V27C60 13 38 13 38 27V53"/><path class="tip" d="M28 51L38 67 48 51Z"/></svg>
                <span class="connection-name">Sağ üstten aşağı · ZZ</span>
              </button>
              <button class="connection-card" type="button" data-connection="7" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M82 60V28C82 11 60 11 60 28V43C60 57 38 57 38 43V17"/><path class="tip" d="M28 19L38 3 48 19Z"/></svg>
                <span class="connection-name">Sağ alttan yukarı · ZZ</span>
              </button>
              <button class="connection-card" type="button" data-connection="8" aria-pressed="false">
                <svg viewBox="0 0 100 70" aria-hidden="true"><path class="route" d="M18 60V28C18 11 40 11 40 28V43C40 57 62 57 62 43V17"/><path class="tip" d="M52 19L62 3 72 19Z"/></svg>
                <span class="connection-name">Sol alttan yukarı · ZZ</span>
              </button>
            </div>
          </div>
          <div class="field full">
            <label>RGB panel testi</label>
            <div class="tests">
              <button type="button" class="button test-red" data-color="red">Kırmızı</button>
              <button type="button" class="button test-green" data-color="green">Yeşil</button>
              <button type="button" class="button test-blue" data-color="blue">Mavi</button>
            </div>
          </div>
        </div>
        <div class="actions"><button class="button" type="submit">Panel Ayarlarını Kaydet</button></div>
      </form>
    </section>

    <section class="panel" id="clock-section">
      <h2>Saat ve Tarih Ayarları</h2>
      <p class="note">Bilgiler DS3231 gerçek zaman saatine yazılır; elektrik kesildiğinde RTC piliyle korunur.</p>
      <form id="clock-form">
        <div class="grid">
          <div class="field"><label for="date">Tarih</label><input id="date" name="date" type="date" required></div>
          <div class="field"><label for="time">Saat</label><input id="time" name="time" type="time" step="1" required></div>
          <fieldset>
            <legend>Panel uyku ayarı</legend>
            <label class="sleep-toggle" for="sleepEnabled">
              <input id="sleepEnabled" name="sleepEnabled" type="checkbox" value="1">
              Belirlenen saatler arasında paneli karart
            </label>
            <div class="field"><label for="sleepStart">Başlangıç saati</label><input id="sleepStart" name="sleepStart" type="time" step="60" required></div>
            <div class="field"><label for="sleepEnd">Bitiş saati</label><input id="sleepEnd" name="sleepEnd" type="time" step="60" required></div>
            <div class="field full"><label for="sleepDayMode">Açılış günleri</label><select id="sleepDayMode" name="sleepDayMode"><option value="0">Haftanın her günü</option><option value="1">Sadece hafta içi</option></select></div>
            <p class="sleep-hint">Panel her gün başlangıç saatinde karartılır. Bitiş saati seçili açılış günlerinin dışında kalırsa panel kapalı kalır ve ilk uygun günün bitiş saatinde açılır. Örneğin cuma 23:00’te kapanan panel, hafta içi seçiminde pazartesi 07:00’de açılır. Cihaz, Wi-Fi ve web arayüzü çalışmaya devam eder.</p>
          </fieldset>
        </div>
        <div class="actions">
          <button class="button secondary" id="browser-time" type="button">Telefon/Bilgisayar Saatini Kullan</button>
          <button class="button" type="submit">Saat ve Tarihi Kaydet</button>
        </div>
      </form>
    </section>

    <section class="panel" id="alarm-section">
      <h2>Alarm Ayarları</h2>
      <p class="note">Alarm tetiklendiğinde IO40 rölesi belirlenen süre boyunca açık kalır. En fazla 10 alarm oluşturabilirsiniz.</p>
      <form id="alarm-form">
        <div class="alarm-summary"><strong id="alarm-count">0 / 10 alarm</strong><span>Süre: 1–3600 saniye</span></div>
        <p class="alarm-empty" id="alarm-empty">Henüz alarm oluşturulmadı.</p>
        <div class="alarm-list" id="alarm-list"></div>
        <div class="actions">
          <button class="button secondary" id="add-alarm" type="button">Yeni Alarm Ekle</button>
          <button class="button" type="submit">Alarm Ayarlarını Kaydet</button>
        </div>
      </form>
    </section>

    <section class="panel" id="communication-section">
      <h2>Haberleşme Ayarları</h2>
      <p class="note">RS485 Modbus RTU hattı RX IO18, TX IO17 ve DIR IO7 pinlerini kullanır. Değişiklikler kaydedildikten sonra cihaz otomatik yeniden başlar.</p>
      <form id="communication-form">
        <div class="grid">
          <div class="field">
            <label for="baudRate">Baudrate</label>
            <select id="baudRate" name="baudRate"><option value="4800">4800</option><option value="9600">9600</option><option value="19200">19200</option><option value="57600">57600</option><option value="115200">115200</option></select>
          </div>
          <div class="field">
            <label for="dataBits">Data size</label>
            <select id="dataBits" name="dataBits"><option value="5">5 bit</option><option value="6">6 bit</option><option value="7">7 bit</option><option value="8">8 bit</option></select>
          </div>
          <div class="field">
            <label for="parity">Parity</label>
            <select id="parity" name="parity"><option value="0">Yok (N)</option><option value="1">Even (E)</option><option value="2">Odd (O)</option></select>
          </div>
          <div class="field">
            <label for="handshake">Handshake / yön kontrolü</label>
            <select id="handshake" name="handshake"><option value="1">RS485 DIR kontrolü (IO7)</option><option value="0">Yok / otomatik yön donanımı</option></select>
          </div>
          <p class="sleep-hint">ST3485 bağlantısında <strong>RS485 DIR kontrolü</strong> kullanılmalıdır. “Yok” seçeneği yalnızca kendi yönünü otomatik değiştiren RS485 dönüştürücüler içindir. Stop bit sabit olarak 1’dir.</p>
        </div>
        <div class="actions"><button class="button" type="submit">Haberleşme Ayarlarını Kaydet</button></div>
      </form>
    </section>

    <section class="panel" id="wifi-section">
      <h2>Wi-Fi Ayarları</h2>
      <p class="note">STA bilgileri girilmişse cihaz bağlantıyı her 60 saniyede bir, en fazla 10 saniye boyunca dener.</p>
      <div class="wifi-connection" id="wifi-connection" data-state="waiting" role="status" aria-live="polite">
        <span class="wifi-dot" aria-hidden="true"></span>
        <span class="wifi-copy">
          <strong id="wifi-state-label">STA durumu alınıyor</strong>
          <span id="wifi-state-detail">Lütfen bekleyin…</span>
          <span class="wifi-signal" id="wifi-signal" hidden>
            <span class="wifi-signal-track" aria-hidden="true"><span id="wifi-signal-fill"></span></span>
            <span class="wifi-signal-text" id="wifi-signal-text"></span>
          </span>
        </span>
        <code class="wifi-ip" id="wifi-ip" hidden></code>
      </div>
      <form id="wifi-form">
        <div class="grid">
          <fieldset>
            <legend>Mevcut cihaz ağı (AP)</legend>
            <div class="field"><label for="apSsid">SSID</label><input id="apSsid" name="apSsid" maxlength="32" required></div>
            <div class="field"><label for="apPassword">Parola</label><input id="apPassword" name="apPassword" type="password" minlength="8" maxlength="63" required></div>
          </fieldset>
          <fieldset>
            <legend>Bağlanılacak ağ (STA)</legend>
            <p class="sta-warning" role="note"><span aria-hidden="true">&#9888;</span><span><strong>Yalnızca 2.4 GHz:</strong> ESP32-S2, 5 GHz Wi-Fi ağlarına bağlanamaz. STA için 2.4 GHz yayın yapan bir SSID seçin.</span></p>
            <div class="field"><label for="staSsid">SSID</label><input id="staSsid" name="staSsid" maxlength="32" placeholder="Boş bırakılırsa devre dışı"></div>
            <div class="field"><label for="staPassword">Parola</label><input id="staPassword" name="staPassword" type="password" maxlength="63" placeholder="Açık ağ için boş bırakılabilir"></div>
          </fieldset>
        </div>
        <div class="actions"><button class="button" type="submit">Wi-Fi Ayarlarını Kaydet</button></div>
      </form>
    </section>

    <section class="panel" id="firmware-section">
      <h2>Yazılım Güncelleme</h2>
      <p class="note">Bilgisayarınızdan veya telefonunuzdan OTA firmware dosyasını seçin. Yükleme tamamlandığında cihaz otomatik olarak yeniden başlar.</p>
      <form id="firmware-form">
        <div class="firmware-card">
          <div class="field">
            <label for="firmwareFile">Firmware dosyası</label>
            <input class="firmware-file" id="firmwareFile" name="firmware" type="file" accept=".bin" required>
            <p class="firmware-meta" id="firmware-file-meta"><strong>Gerekli dosya adı:</strong> firmware.bin</p>
          </div>
          <div class="firmware-progress" id="firmware-progress" hidden>
            <div class="firmware-progress-track" id="firmware-progress-track" role="progressbar" aria-label="Firmware yükleme ilerlemesi" aria-valuemin="0" aria-valuemax="100" aria-valuenow="0"><span class="firmware-progress-bar" id="firmware-progress-bar"></span></div>
            <output class="firmware-progress-value" id="firmware-progress-value">0%</output>
            <p class="firmware-progress-text" id="firmware-progress-text" aria-live="polite">Yükleme bekleniyor.</p>
          </div>
        </div>
        <div class="actions"><button class="button" id="firmware-submit" type="submit" disabled>Güncellemeyi Başlat</button></div>
      </form>
    </section>

    <footer>Yazılım v)HTML"
FIRMWARE_VERSION
R"HTML(</footer>
  </div>
  <div class="status" id="status" role="status" aria-live="polite"></div>
  <script>
    const byId = id => document.getElementById(id);
    const statusBox = byId('status');
    let statusTimer;
    let firmwareUploadActive = false;

    function showStatus(message, error = false) {
      clearTimeout(statusTimer);
      statusBox.textContent = message;
      statusBox.className = 'status show' + (error ? ' error' : '');
      statusTimer = setTimeout(() => statusBox.className = 'status', 4500);
    }

    document.querySelectorAll('.tab').forEach(button => button.addEventListener('click', () => {
      document.querySelectorAll('.tab').forEach(item => item.classList.toggle('active', item === button));
      document.querySelectorAll('.panel').forEach(item => item.classList.toggle('active', item.id === button.dataset.target + '-section'));
    }));

    let maximumAlarms = 10;

    function refreshAlarmList() {
      const cards = [...document.querySelectorAll('.alarm-card')];
      cards.forEach((card, index) => {
        card.querySelector('.alarm-title').textContent = `Alarm ${index + 1}`;
        card.querySelector('.alarm-time').name = `alarmTime${index}`;
        card.querySelector('.alarm-duration').name = `alarmDuration${index}`;
        card.querySelector('.alarm-days').name = `alarmDayMode${index}`;
        card.querySelector('.alarm-day-mask').name = `alarmDayMask${index}`;
        updateAlarmDayControls(card);
      });
      byId('alarm-count').textContent = `${cards.length} / ${maximumAlarms} alarm`;
      byId('alarm-empty').hidden = cards.length !== 0;
      byId('add-alarm').disabled = cards.length >= maximumAlarms;
    }

    function addAlarm(item = {}) {
      const list = byId('alarm-list');
      if (list.children.length >= maximumAlarms) {
        showStatus(`En fazla ${maximumAlarms} alarm oluşturabilirsiniz.`, true);
        return;
      }

      const card = document.createElement('div');
      card.className = 'alarm-card';
      card.innerHTML = `
        <div class="alarm-card-head">
          <strong class="alarm-title">Alarm</strong>
          <button class="alarm-remove" type="button">Sil</button>
        </div>
        <div class="alarm-fields">
          <div class="field"><label>Alarm saati</label><input class="alarm-time" type="time" step="60" required></div>
          <div class="field"><label>Tetiklenme süresi (saniye)</label><input class="alarm-duration" type="number" min="1" max="3600" required></div>
          <div class="field"><label>Tekrarlama günleri</label><select class="alarm-days"><option value="0">Haftanın her günü</option><option value="1">Sadece hafta içi</option><option value="2">İstediğim günler</option></select></div>
          <div class="alarm-custom-days" hidden>
            <p class="sleep-hint">Alarmın çalışacağı günleri seçin.</p>
            <div class="alarm-day-grid">
              <label class="alarm-day-choice"><input type="checkbox" data-alarm-day="0"><span>Pazartesi</span></label>
              <label class="alarm-day-choice"><input type="checkbox" data-alarm-day="1"><span>Salı</span></label>
              <label class="alarm-day-choice"><input type="checkbox" data-alarm-day="2"><span>Çarşamba</span></label>
              <label class="alarm-day-choice"><input type="checkbox" data-alarm-day="3"><span>Perşembe</span></label>
              <label class="alarm-day-choice"><input type="checkbox" data-alarm-day="4"><span>Cuma</span></label>
              <label class="alarm-day-choice"><input type="checkbox" data-alarm-day="5"><span>Cumartesi</span></label>
              <label class="alarm-day-choice"><input type="checkbox" data-alarm-day="6"><span>Pazar</span></label>
            </div>
            <input class="alarm-day-mask" type="hidden" value="127">
          </div>
        </div>`;

      const pad = value => String(value).padStart(2, '0');
      const hour = item.hour === undefined ? 8 : Math.max(0, Math.min(23, Number(item.hour)));
      const minute = item.minute === undefined ? 0 : Math.max(0, Math.min(59, Number(item.minute)));
      card.querySelector('.alarm-time').value = `${pad(hour)}:${pad(minute)}`;
      card.querySelector('.alarm-duration').value = item.duration === undefined ? 5 : item.duration;
      const dayMode = [0, 1, 2].includes(Number(item.dayMode)) ? Number(item.dayMode) : 0;
      const defaultDayMask = dayMode === 1 ? 31 : 127;
      const dayMask = Number.isInteger(Number(item.dayMask))
        && Number(item.dayMask) > 0
        && Number(item.dayMask) <= 127
        ? Number(item.dayMask)
        : defaultDayMask;
      card.querySelector('.alarm-days').value = String(dayMode);
      card.querySelectorAll('[data-alarm-day]').forEach(input => {
        input.checked = (dayMask & (1 << Number(input.dataset.alarmDay))) !== 0;
        input.addEventListener('change', () => {
          if (alarmDayMask(card) === 0) {
            input.checked = true;
            showStatus('Alarm için en az bir gün seçilmelidir.', true);
          }
          updateAlarmDayControls(card);
        });
      });
      card.querySelector('.alarm-days').addEventListener('change', () => updateAlarmDayControls(card));
      updateAlarmDayControls(card);
      card.querySelector('.alarm-remove').addEventListener('click', () => {
        card.remove();
        refreshAlarmList();
      });
      list.appendChild(card);
      refreshAlarmList();
    }

    function alarmDayMask(card) {
      return [...card.querySelectorAll('[data-alarm-day]')].reduce(
        (mask, input) => input.checked
          ? mask | (1 << Number(input.dataset.alarmDay))
          : mask,
        0);
    }

    function updateAlarmDayControls(card) {
      const custom = Number(card.querySelector('.alarm-days').value) === 2;
      card.querySelector('.alarm-custom-days').hidden = !custom;
      const maskInput = card.querySelector('.alarm-day-mask');
      const mask = String(alarmDayMask(card));
      maskInput.value = mask;
      maskInput.setAttribute('value', mask);
    }

    function renderAlarms(data) {
      maximumAlarms = Math.max(1, Number(data && data.maximum) || 10);
      byId('alarm-list').replaceChildren();
      const items = data && Array.isArray(data.items) ? data.items : [];
      items.slice(0, maximumAlarms).forEach(addAlarm);
      refreshAlarmList();
    }

    function selectConnection(value) {
      const selectedValue = String(value);
      byId('connectionStyle').value = selectedValue;
      document.querySelectorAll('.connection-card').forEach(card => {
        const selected = card.dataset.connection === selectedValue;
        card.classList.toggle('selected', selected);
        card.setAttribute('aria-pressed', selected ? 'true' : 'false');
      });
    }

    document.querySelectorAll('.connection-card').forEach(card => {
      card.addEventListener('click', () => selectConnection(card.dataset.connection));
    });

    function updatePanelCount() {
      byId('count').value = (Number(byId('rows').value) || 0) * (Number(byId('columns').value) || 0);
    }

    function limitPanelDimension(event) {
      if (event.target.value === '') return updatePanelCount();
      event.target.value = Math.min(4, Math.max(1, Number(event.target.value) || 1));
      updatePanelCount();
    }

    byId('rows').addEventListener('input', limitPanelDimension);
    byId('columns').addEventListener('input', limitPanelDimension);
    byId('brightness').addEventListener('input', event => byId('brightness-value').value = event.target.value);

    async function request(url, options = {}) {
      const response = await fetch(url, {cache: 'no-store', ...options});
      const data = await response.json();
      if (!response.ok || ('ok' in data && !data.ok)) throw new Error(data.message || 'İşlem başarısız.');
      return data;
    }

    const wifiStateLabels = {
      disabled: 'STA devre dışı',
      waiting: 'Bağlantı bekleniyor',
      paused: 'STA bağlantısı bekletiliyor',
      connecting: 'Ağa bağlanılıyor',
      connected: 'STA bağlantısı başarılı',
      failed: 'STA bağlantısı başarısız'
    };

    const wifiFailureLabels = {
      no_ssid: 'Kaydedilen Wi-Fi ağı bulunamadı.',
      authentication: 'Erişim noktası bağlantıyı kabul etmedi; SSID ve parolayı kontrol edin.',
      connection_lost: 'Kablosuz bağlantı kesildi.',
      timeout: '10 saniye içinde bağlantı kurulamadı.',
      status_unavailable: 'STA bağlantı durumu cihazdan alınamadı.'
    };

    let wifiStatusData = null;
    let wifiRetrySeconds = 0;

    function wifiRetryText() {
      return wifiRetrySeconds > 0
        ? ` Sonraki denemeye ${wifiRetrySeconds} saniye kaldı.`
        : ' Yeni bağlantı denemesi başlatılıyor…';
    }

    function updateWifiStatusDetail() {
      if (!wifiStatusData) return;
      const data = wifiStatusData;
      const state = wifiStateLabels[data.state] ? data.state : 'waiting';
      let detail = 'Bir sonraki otomatik bağlantı denemesi bekleniyor.' + wifiRetryText();
      if (state === 'disabled') detail = 'Bağlanılacak bir STA ağı kaydedilmemiş.';
      if (state === 'paused') detail = 'Yerel AP arayüzüne bir cihaz bağlı olduğu için yeni STA denemeleri bekletiliyor.';
      if (state === 'connecting') detail = `“${data.ssid}” ağına bağlanılıyor…`;
      if (state === 'connected') detail = `“${data.ssid}” ağına bağlandı.`;
      if (state === 'failed') {
        detail = wifiFailureLabels[data.reason] || 'Bağlantı kurulamadı.';
        if (data.reason !== 'status_unavailable') detail += wifiRetryText();
      }
      byId('wifi-state-detail').textContent = detail;
    }

    function renderWifiStatus(data) {
      const box = byId('wifi-connection');
      const state = wifiStateLabels[data.state] ? data.state : 'waiting';
      wifiStatusData = data;
      wifiRetrySeconds = Math.max(0, Number(data.retryInSeconds) || 0);
      box.dataset.state = state;
      byId('wifi-state-label').textContent = wifiStateLabels[state];
      updateWifiStatusDetail();

      const ip = byId('wifi-ip');
      ip.textContent = data.ip || '';
      ip.hidden = state !== 'connected' || !data.ip;

      const signal = byId('wifi-signal');
      const rssiAvailable = data.rssi !== null && data.rssi !== undefined && Number.isFinite(Number(data.rssi));
      const signalAvailable = state === 'connected' && rssiAvailable;
      signal.hidden = !signalAvailable;
      if (signalAvailable) {
        const rssi = Math.round(Number(data.rssi));
        const quality = Math.max(0, Math.min(100, Math.round(Number(data.quality) || 0)));
        const fill = byId('wifi-signal-fill');
        fill.style.width = `${quality}%`;
        fill.style.backgroundColor = quality >= 70 ? '#32ca70' : quality >= 40 ? '#f0ad3d' : '#ef4c5b';
        byId('wifi-signal-text').textContent = `Sinyal ${quality}% (${rssi} dBm)`;
      }
    }

    async function loadWifiStatus() {
      if (firmwareUploadActive) return;
      try {
        renderWifiStatus(await request('/api/wifi/status'));
      } catch (error) {
        renderWifiStatus({state:'failed', reason:'status_unavailable', ssid:'', ip:'', retryInSeconds:0, rssi:null, quality:0});
      }
    }

    function updateWifiCountdown() {
      if (!wifiStatusData) return;
      if (wifiRetrySeconds > 0) wifiRetrySeconds--;
      if (wifiStatusData.state === 'failed' || wifiStatusData.state === 'waiting') {
        updateWifiStatusDetail();
      }
    }

    async function loadSettings() {
      try {
        const data = await request('/api/settings');
        byId('scan').value = data.panel.scan;
        byId('rows').value = data.panel.rows;
        byId('columns').value = data.panel.columns;
        byId('brightness').value = data.panel.brightness;
        byId('brightness-value').value = data.panel.brightness;
        selectConnection(data.panel.connectionStyle);
        updatePanelCount();
        if (data.clock.available) {
          byId('date').value = data.clock.date;
          byId('time').value = data.clock.time;
        }
        byId('sleepEnabled').checked = Boolean(data.clock.sleepEnabled);
        byId('sleepStart').value = data.clock.sleepStart;
        byId('sleepEnd').value = data.clock.sleepEnd;
        byId('sleepDayMode').value = data.clock.sleepDayMode;
        renderAlarms(data.alarms);
        byId('baudRate').value = data.communication.baudRate;
        byId('dataBits').value = data.communication.dataBits;
        byId('parity').value = data.communication.parity;
        byId('handshake').value = data.communication.handshake;
        byId('apSsid').value = data.wifi.apSsid;
        byId('apPassword').value = data.wifi.apPassword;
        byId('staSsid').value = data.wifi.staSsid;
        byId('staPassword').value = data.wifi.staPassword;
      } catch (error) { showStatus(error.message, true); }
    }

    async function submitForm(event, endpoint) {
      event.preventDefault();
      try {
        if (endpoint === '/api/settings/alarms') {
          refreshAlarmList();
          const invalidCustomDays = [...document.querySelectorAll('.alarm-card')].some(card =>
            Number(card.querySelector('.alarm-days').value) === 2
            && alarmDayMask(card) === 0);
          if (invalidCustomDays) {
            throw new Error('Özel alarm için en az bir gün seçilmelidir.');
          }
        }
        const body = new URLSearchParams(new FormData(event.currentTarget));
        if (endpoint === '/api/settings/clock') {
          body.set('sleepEnabled', byId('sleepEnabled').checked ? '1' : '0');
        }
        if (endpoint === '/api/settings/alarms') {
          const alarmCards = [...document.querySelectorAll('.alarm-card')];
          body.set('alarmCount', alarmCards.length);
          alarmCards.forEach((card, index) => {
            body.set(`alarmDayMask${index}`, String(alarmDayMask(card)));
          });
        }
        const result = await request(endpoint, {method: 'POST', headers: {'Content-Type':'application/x-www-form-urlencoded'}, body});
        showStatus(result.message);
      } catch (error) { showStatus(error.message, true); }
    }

    byId('panel-form').addEventListener('submit', event => submitForm(event, '/api/settings/panel'));
    byId('clock-form').addEventListener('submit', event => submitForm(event, '/api/settings/clock'));
    byId('alarm-form').addEventListener('submit', event => submitForm(event, '/api/settings/alarms'));
    byId('communication-form').addEventListener('submit', event => submitForm(event, '/api/settings/communication'));
    byId('wifi-form').addEventListener('submit', event => submitForm(event, '/api/settings/wifi'));
    byId('add-alarm').addEventListener('click', () => addAlarm());

    const firmwareForm = byId('firmware-form');
    const firmwareFile = byId('firmwareFile');
    const firmwareSubmit = byId('firmware-submit');

    function firmwareFileSize(bytes) {
      if (bytes < 1024) return `${bytes} bayt`;
      if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
      return `${(bytes / (1024 * 1024)).toFixed(2)} MB`;
    }

    function setFirmwareProgress(percent, message) {
      const value = Math.max(0, Math.min(100, Math.round(percent)));
      byId('firmware-progress').hidden = false;
      byId('firmware-progress-bar').style.width = `${value}%`;
      byId('firmware-progress-value').value = `${value}%`;
      byId('firmware-progress-track').setAttribute('aria-valuenow', String(value));
      byId('firmware-progress-text').textContent = message;
    }

    function validateFirmwareFile(showError = false) {
      const file = firmwareFile.files[0];
      const valid = Boolean(file && file.name === 'firmware.bin');
      firmwareSubmit.disabled = firmwareUploadActive || !valid;

      if (!file) {
        byId('firmware-file-meta').innerHTML = '<strong>Gerekli dosya adı:</strong> firmware.bin';
      } else if (valid) {
        byId('firmware-file-meta').textContent = `${file.name} • ${firmwareFileSize(file.size)}`;
      } else {
        byId('firmware-file-meta').innerHTML = '<strong>Geçersiz dosya.</strong> Dosya adı tam olarak firmware.bin olmalıdır.';
        if (showError) showStatus('Dosya adı tam olarak firmware.bin olmalıdır.', true);
      }
      return valid;
    }

    firmwareFile.addEventListener('change', () => validateFirmwareFile(true));

    firmwareForm.addEventListener('submit', event => {
      event.preventDefault();
      if (!validateFirmwareFile(true)) return;

      const file = firmwareFile.files[0];
      const body = new FormData();
      body.append('firmware', file, file.name);
      const xhr = new XMLHttpRequest();
      firmwareUploadActive = true;
      firmwareFile.disabled = true;
      firmwareSubmit.disabled = true;
      setFirmwareProgress(0, 'Firmware yüklenmeye hazırlanıyor…');

      xhr.open('POST', '/api/firmware/update');
      xhr.responseType = 'json';
      xhr.upload.addEventListener('progress', progress => {
        if (!progress.lengthComputable) return;
        const percent = progress.total ? progress.loaded * 100 / progress.total : 0;
        setFirmwareProgress(percent, percent >= 100 ? 'Firmware doğrulanıyor…' : 'Firmware cihaza yükleniyor…');
      });
      xhr.addEventListener('load', () => {
        const result = xhr.response || {};
        if (xhr.status >= 200 && xhr.status < 300 && result.ok) {
          setFirmwareProgress(100, result.message || 'Güncelleme tamamlandı. Cihaz yeniden başlatılıyor.');
          showStatus(result.message || 'Güncelleme tamamlandı.');
          return;
        }

        firmwareUploadActive = false;
        firmwareFile.disabled = false;
        validateFirmwareFile();
        const message = result.message || 'Firmware güncellenemedi.';
        setFirmwareProgress(0, message);
        showStatus(message, true);
      });
      xhr.addEventListener('error', () => {
        firmwareUploadActive = false;
        firmwareFile.disabled = false;
        validateFirmwareFile();
        setFirmwareProgress(0, 'Cihaza bağlantı kesildi. Firmware güncellenemedi.');
        showStatus('Cihaza bağlantı kesildi. Firmware güncellenemedi.', true);
      });
      xhr.addEventListener('abort', () => {
        firmwareUploadActive = false;
        firmwareFile.disabled = false;
        validateFirmwareFile();
        setFirmwareProgress(0, 'Firmware yüklemesi iptal edildi.');
      });
      xhr.send(body);
    });

    document.querySelectorAll('[data-color]').forEach(button => button.addEventListener('click', async () => {
      try {
        const result = await request('/api/panel/test?color=' + button.dataset.color, {method: 'POST'});
        showStatus(result.message);
      } catch (error) { showStatus(error.message, true); }
    }));

    byId('browser-time').addEventListener('click', () => {
      const now = new Date();
      const pad = value => String(value).padStart(2, '0');
      byId('date').value = `${now.getFullYear()}-${pad(now.getMonth() + 1)}-${pad(now.getDate())}`;
      byId('time').value = `${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}`;
    });

    loadSettings();
    loadWifiStatus();
    setInterval(loadWifiStatus, 2000);
    setInterval(updateWifiCountdown, 1000);
  </script>
</body>
</html>)HTML";

} // namespace PortalPage
