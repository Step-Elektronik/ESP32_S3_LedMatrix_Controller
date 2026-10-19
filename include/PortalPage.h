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
    .top-actions { display: flex; align-items: center; gap: 9px; }
    .top-actions .language-select {
      width: auto; min-height: 46px; padding: 9px 34px 9px 12px;
      color: #dce9ff; border: 1px solid rgba(255,255,255,.13); border-radius: 14px;
      background: #102139; cursor: pointer;
    }
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
    .segments.type-segments { grid-template-columns: repeat(2,minmax(0,1fr)); }
    .segment:disabled { opacity: .4; cursor: not-allowed; }
    .cycle-choice input:disabled + span { opacity: .4; cursor: not-allowed; }
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
    .three-col { display: grid; grid-template-columns: repeat(3,minmax(0,1fr)); gap: 8px; }
    .three-col > div { display: flex; flex-direction: column; justify-content: flex-end; }
    .countdown-card { margin-bottom: 18px; }
    .countdown-body { padding: 16px 18px 18px; }
    .countdown-times { display: flex; flex-wrap: wrap; gap: 10px; margin-bottom: 14px; }
    .countdown-time { flex: 1 1 170px; padding: 12px 14px; border: 1px solid rgba(255,255,255,.10); border-radius: 14px; background: rgba(0,0,0,.22); }
    .countdown-time span { display: block; color: #8195b2; font-size: 11px; font-weight: 800; }
    .countdown-time b { display: block; margin-top: 4px; font: 800 clamp(32px,7vw,46px)/1.05 ui-monospace, SFMono-Regular, Menlo, Consolas, monospace; font-variant-numeric: tabular-nums; }
    .countdown-alarm { margin-bottom: 12px; padding: 9px 12px; color: #ffd27a; font-size: 13px; font-weight: 800; border: 1px solid rgba(255,190,70,.35); border-radius: 12px; background: rgba(255,170,30,.12); }
    .countdown-actions { display: grid; grid-template-columns: repeat(3,1fr); gap: 9px; }
    .countdown-status { display: block; min-height: 16px; margin-top: 10px; }
    .panel-pill.running { color: #8ff0c8; background: rgba(40,200,140,.16); }
    .panel-pill.paused { color: #ffd27a; background: rgba(255,170,30,.14); }
    .panel-pill.finished { color: #ffb5bd; background: rgba(255,52,77,.14); }
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
    html[dir="rtl"] body { direction: rtl; }
    html[dir="rtl"] .stage,
    html[dir="rtl"] input[type="number"],
    html[dir="rtl"] .coords,
    html[dir="rtl"] .countdown-time b { direction: ltr; }
    html[dir="rtl"] .countdown-time b { text-align: right; }
    html[dir="rtl"] .slider::after { left: auto; right: 3px; }
    html[dir="rtl"] .switch input:checked + .slider::after { transform: translateX(-20px); }
    @media (max-width: 850px) { .layout { grid-template-columns: 1fr; } }
    @media (max-width: 540px) {
      .shell { padding: 14px 12px 28px; } .subtitle { display: none; }
      .topbar { flex-wrap: wrap; } .top-actions { width: 100%; justify-content: flex-end; }
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
      <div class="top-actions">
        <select class="language-select" id="languageSelect" aria-label="Dil" title="Dil">
          <option value="tr">Türkçe</option><option value="en">English</option><option value="de">Deutsch</option><option value="es">Español</option><option value="ar">العربية</option>
        </select>
        <a class="settings" href="/settings" aria-label="Ayarlar" title="Ayarlar">
          <svg viewBox="0 0 24 24" aria-hidden="true"><path d="M19.14 12.94c.04-.31.06-.63.06-.94s-.02-.63-.07-.94l2.03-1.58a.49.49 0 0 0 .12-.61l-1.92-3.32a.49.49 0 0 0-.59-.22l-2.39.96a7.08 7.08 0 0 0-1.62-.94L14.4 2.8a.48.48 0 0 0-.48-.4h-3.84a.48.48 0 0 0-.48.4l-.36 2.54c-.58.24-1.12.56-1.62.94l-2.39-.96a.48.48 0 0 0-.59.22L2.72 8.87a.48.48 0 0 0 .12.61l2.03 1.58c-.05.31-.08.64-.08.94s.03.63.08.94l-2.03 1.58a.49.49 0 0 0-.12.61l1.92 3.32c.12.22.38.31.59.22l2.39-.96c.5.39 1.04.71 1.62.95l.36 2.54c.04.23.24.4.48.4h3.84c.24 0 .44-.17.48-.4l.36-2.54c.58-.24 1.12-.56 1.62-.95l2.39.96c.21.09.47 0 .59-.22l1.92-3.32a.49.49 0 0 0-.12-.61l-2.02-1.58ZM12 15.5A3.5 3.5 0 1 1 12 8a3.5 3.5 0 0 1 0 7.5Z"/></svg>
        </a>
      </div>
    </header>

    <section class="card countdown-card" id="countdownCard" aria-labelledby="countdown-title" hidden>
      <div class="card-head">
        <div><h2 class="card-title" id="countdown-title">Geri Sayım</h2><p class="card-note">Panele uygulanmış Geri Sayım kutularını başlatın, durdurun veya sıfırlayın</p></div>
        <span class="panel-pill" id="countdownState">Hazır</span>
      </div>
      <div class="countdown-body">
        <div class="countdown-times" id="countdownTimes"></div>
        <div class="countdown-alarm" id="countdownAlarmActive" hidden>Süre doldu, alarm rölesi çalışıyor. Durdur veya Sıfırla alarmı susturur.</div>
        <div class="countdown-actions">
          <button class="btn primary" id="countdownStart" type="button">Başlat</button>
          <button class="btn" id="countdownPause" type="button">Durdur</button>
          <button class="btn danger" id="countdownReset" type="button">Sıfırla</button>
        </div>
        <span class="status countdown-status" id="countdownStatus"></span>
      </div>
    </section>

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
            <div class="segments type-segments" id="typeSegments">
              <button class="segment" type="button" data-type="0">Yazı</button>
              <button class="segment" type="button" data-type="1">Saat</button>
              <button class="segment" type="button" data-type="2">Tarih</button>
              <button class="segment" type="button" data-type="3">Sıcaklık</button>
              <button class="segment" type="button" data-type="4">Nem</button>
              <button class="segment" type="button" data-type="5">Geri Sayım</button>
            </div>
            <div class="card-note" id="humidityNote" hidden></div>
          </div>
          <div class="field switch-row">
            <div><span class="field-title">İçerik döngüsü</span><div class="card-note">Birden fazla bilgiyi aynı TextBox'ta sırayla gösterir</div></div>
            <label class="switch"><input id="cycleEnabled" type="checkbox"><span class="slider"></span></label>
          </div>
          <p class="cycle-note" id="cycleCountdownNote" hidden>Geri sayım içerik döngüsüne eklenemez.</p>
          <div class="cycle-options" id="cycleOptions" hidden>
            <p class="cycle-note">İşaretlenen bilgiler Yazı → Saat → Tarih → Sıcaklık → Nem sırasıyla gösterilir. Kayar yazı açıksa içerik tam turunu tamamladıktan sonra değişir.</p>
            <div class="cycle-choices">
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="0"><span>Yazı</span></label>
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="1"><span>Saat</span></label>
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="2"><span>Tarih</span></label>
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="3"><span>Sıcaklık</span></label>
              <label class="cycle-choice"><input type="checkbox" data-cycle-type="4"><span>Nem</span></label>
            </div>
            <div class="field" style="margin-bottom:0"><label for="cycleInterval">Değişim sıklığı (saniye)</label><input id="cycleInterval" type="number" min="1" max="3600" value="5" inputmode="numeric"></div>
          </div>
          <div class="field">
            <label for="textValue">Yazdırılacak metin</label>
            <input id="textValue" type="text" maxlength="40" autocomplete="off" placeholder="Metninizi yazın">
            <div class="card-note">Türkçe, Almanca ve İspanyolca harfler ile ° işareti panelde gösterilir; ß “ss” olarak yazılır. Diğer karakterler “?” olarak görünür.</div>
          </div>
          <div class="field" id="temperatureFormatOptions" hidden>
            <label for="temperatureFormat">Sıcaklık ve nem gösterimi</label>
            <select id="temperatureFormat">
              <option value="0">Ondalıklı — bir basamak (23.5°C, 45.3%)</option>
              <option value="1">Tam sayı — yuvarlanmış (24°C, 45%)</option>
            </select>
            <div class="card-note">Tam sayı seçiminde en yakın tam sayıya yuvarlanır; 0.5 ve üzeri bir üst değere çıkar.</div>
          </div>
          <div id="countdownOptions" hidden>
            <div class="field">
              <span class="field-title">Geri sayım süresi</span>
              <div class="three-col">
                <div><label for="durationHours">Saat (0-23)</label><input id="durationHours" type="number" min="0" max="23" step="1" inputmode="numeric"></div>
                <div><label for="durationMinutes">Dakika (0-59)</label><input id="durationMinutes" type="number" min="0" max="59" step="1" inputmode="numeric"></div>
                <div><label for="durationSeconds">Saniye (0-59)</label><input id="durationSeconds" type="number" min="0" max="59" step="1" inputmode="numeric"></div>
              </div>
              <div class="card-note">Saat 0 ise panelde yalnızca dakika ve saniye (40:00), saat ve dakika 0 ise yalnızca saniye (45) gösterilir.</div>
            </div>
            <div class="field">
              <label for="countdownAlarm">Alarm (röle) süresi — saniye</label>
              <input id="countdownAlarm" type="number" min="0" max="3600" step="1" inputmode="numeric">
              <div class="card-note">Süre bittiğinde alarm rölesi bu kadar çalışır; 0 alarmı kapatır. Panele uyguladıktan sonra Başlat, Durdur ve Sıfırla sayfanın üstündeki Geri Sayım kartında görünür.</div>
            </div>
          </div>
          </div>
          <div id="valueBoxOptions" hidden>
            <div class="field">
              <label for="initialValue">Başlangıç değeri</label>
              <input id="initialValue" type="number" step="1" value="0" inputmode="decimal">
              <div class="card-note">Cihaz her açıldığında ValueBox bu değerden başlar. Tasarım kaydedildiğinde veri kaynağı ve başlangıç değeri değişmeyen kutular mevcut değerini korur.</div>
            </div>
            <div class="two-col">
              <div class="field"><label for="valueDecimals">Ondalık basamak</label><select id="valueDecimals"><option value="0">0</option><option value="1">1</option><option value="2">2</option><option value="3">3</option></select></div>
              <div class="field"><label for="valueSuffix">Birim / son ek</label><input id="valueSuffix" type="text" maxlength="15" autocomplete="off" placeholder="°C, bar, kg"></div>
            </div>
            <p class="cycle-note">Float değer bu basamak sayısına yuvarlanır. Integer değerde ondalık nokta kaydırılır; örneğin 1234 ve 2 basamak 12.34 olarak gösterilir.</p>
            <div class="field">
              <span class="field-title">Veri kaynağı</span>
              <div class="segments value-source-segments">
                <button class="segment" type="button" data-value-source="0">Modbus RTU</button>
                <button class="segment" type="button" data-value-source="1">Dijital giriş</button>
              </div>
            </div>
            <div id="modbusOptions">
              <p class="cycle-note">Slave ID ve seri iletişim seçenekleri Haberleşme Ayarları bölümünden değiştirilebilir. IEEE-754 float seçilen adresten başlayarak iki register kullanır (yüksek word önce).</p>
              <div class="two-col">
                <div class="field"><label for="registerAddress">Register adresi</label><input id="registerAddress" type="number" min="0" max="65535" value="0" inputmode="numeric"></div>
                <div class="field"><label for="valueType">Değer tipi</label><select id="valueType"><option value="0">Signed integer (16-bit)</option><option value="1">Float (IEEE-754)</option></select></div>
              </div>
            </div>
            <div id="digitalInputOptions" hidden>
              <div class="field"><label for="counterDirection">Sayaç yönü</label><select id="counterDirection"><option value="0">Yukarı sayıcı</option><option value="1">Aşağı sayıcı</option></select></div>
              <p class="cycle-note">IO12 dahili pull-down ile LOW seviyesinde bekler. Debounce sonrasındaki LOW → HIGH yükselen kenarında değer artırılır veya azaltılır. Negatif değerler gösterilebilir.</p>
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
            <label for="itemScrollArea">Gösterim alanı</label>
            <select id="itemScrollArea"><option value="0">Yalnızca kendi paneli</option><option value="1">Tüm ekran</option></select>
            <div class="card-note">Kutu bu alanın dışında kırpılır; kayar yazı bu alanın kenarlarında döner.</div>
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
    <footer><span class="version">STEP-RGB • <span>Yazılım v</span>)HTML"
FIRMWARE_VERSION
R"HTML(</span></footer>
  </div>

  <script src="/i18n.js"></script>
  <script>
    (() => {
      'use strict';
      const I18n = window.StepRgbI18n;
      I18n.init();
      const t = (source, values) => I18n.t(source, values);
      const PW = 32, PH = 16;
      const state = { rows:1, columns:1, maxPerPanel:2, fonts:[], defaultFont:4, fontData:new Map(), fontLoads:new Map(), items:[], selectedPanel:0, selectedId:null, nextId:1, drag:null, temperatureValue:null, humidityValue:null,
        sensor:{type:1, name:'TMP112', humidity:false, missingText:'TMP?'},
        countdown:{state:'idle', elapsedMs:0, receivedAt:0, alarmActive:false, items:[]} };
      const $ = id => document.getElementById(id);
      const stage = $('stage'), grid = $('panelGrid'), layer = $('itemLayer');
      const status = $('status'), addTextButton = $('addTextBox'), addValueButton = $('addValueBox'), saveButton = $('saveAll');
      const typeNames = ['Yazı','Saat','Tarih','Sıcaklık','Nem','Geri Sayım'];
      const humidityMask = 1<<4;
      const countdownType = 5, defaultCountdownSeconds = 40*60, defaultCountdownAlarm = 10, maximumCountdownSeconds = 23*3600+59*60+59;
      const countdownStateNames = {idle:'Hazır', running:'Sayıyor', paused:'Durduruldu', finished:'Süre doldu'};
      const countdownStatus = $('countdownStatus');
      const boxNames = ['TextBox','ValueBox'];
      const directionArrows = ['←','→','↑','↓'];

      function setStatus(message, kind='', values={}, element=status) {
        element.dataset.i18nSource = message;
        element.dataset.i18nValues = JSON.stringify(values);
        element.textContent = t(message, values);
        element.className = 'status' + (element===countdownStatus ? ' countdown-status' : '') + (kind ? ' ' + kind : '');
      }
      function retranslateStatus(element) {
        const source=element.dataset.i18nSource;
        if(!source) return;
        let values={};
        try { values=JSON.parse(element.dataset.i18nValues||'{}'); } catch (_) {}
        element.textContent=t(source,values);
      }
      function countdownElapsed() {
        const session=state.countdown;
        return session.state==='running' ? session.elapsedMs+Math.max(0,Date.now()-session.receivedAt) : session.elapsedMs;
      }
      // Mirrors CountdownApplication::format on the device: the configured
      // duration decides whether hours and minutes are shown.
      function formatCountdown(duration,elapsedMs) {
        const remaining=Math.max(0,Math.ceil((duration*1000-elapsedMs)/1000));
        const two=value=>String(value).padStart(2,'0'), seconds=remaining%60;
        if(duration>=3600) return Math.floor(remaining/3600)+':'+two(Math.floor(remaining/60)%60)+':'+two(seconds);
        if(duration>=60) return two(Math.floor(remaining/60))+':'+two(seconds);
        return two(seconds);
      }
      function width() { return state.columns * PW; }
      function height() { return state.rows * PH; }
      function panelCount(index) { return state.items.filter(item => item.panel === index).length; }
      function selectedItem() { return state.items.find(item => item.id === state.selectedId) || null; }
      function toHex(value) { return Number(value).toString(16).padStart(2,'0'); }
      function itemColor(item) { return '#' + toHex(item.red) + toHex(item.green) + toHex(item.blue); }
      function cycleTypes(item) { return [0,1,2,3,4].filter(type => (item.cycleMask & (1<<type)) !== 0); }
      function usesHumidity(item) { return item.kind===0 && (item.cycleEnabled ? (item.cycleMask&humidityMask)!==0 : item.type===4); }
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
        // Mirrors PanelContentApplication::buildText on the device.
        if (type === 3) {
          if (!Number.isFinite(state.temperatureValue)) return state.sensor.missingText;
          const value=Number(state.temperatureValue);
          const rounded=value<0?-Math.round(-value):Math.round(value);
          return (item.temperatureFormat===1?String(rounded):value.toFixed(1))+'°C';
        }
        if (type === countdownType) return formatCountdown(Number(item.countdownSeconds)||0,countdownElapsed());
        if (type === 4) {
          if (!state.sensor.humidity) return '--%';
          if (!Number.isFinite(state.humidityValue)) return state.sensor.missingText;
          const value=Number(state.humidityValue);
          return (item.temperatureFormat===1?String(Math.round(value)):value.toFixed(1))+'%';
        }
        return item.text || 'YAZI';
      }
      function usesFloat(item) { return item.valueSource===0&&item.valueType===1; }
      function formatNumber(item,value) {
        // Mirrors ValueBoxApplication::formatValue on the device.
        const decimals=Math.max(0,Math.min(3,Number(item.decimals)||0));
        if(!Number.isFinite(value)) return 'nan';
        if(usesFloat(item)) return value.toFixed(decimals);
        const integer=Math.trunc(value);
        if(!decimals) return String(integer);
        const magnitude=Math.abs(integer), divisor=10**decimals;
        return (integer<0?'-':'')+Math.floor(magnitude/divisor)+'.'+String(magnitude%divisor).padStart(decimals,'0');
      }
      function liveText(item) {
        if(item.kind!==1) return textForType(item,activeType(item));
        const current=Number.isFinite(item.currentRaw)?item.currentRaw:Number(item.initialValue)||0;
        return formatNumber(item,current)+(item.suffix||'');
      }
      function fontName(item) { return state.fonts[item.font] || state.fonts[state.defaultFont] || 'GohuFont6x11'; }
      // Private glyph codes shared with FontGfx_apple6x10.h on the device.
      const extendedGlyphCodes = {'Ç':0x7F,'Ğ':0x80,'İ':0x81,'Ö':0x82,'Ş':0x83,'Ü':0x84,'ç':0x85,'ğ':0x86,'ı':0x87,'ö':0x88,'ş':0x89,'ü':0x8A,'°':0x8B,
        'Ä':0x8C,'ä':0x8D,'Á':0x8E,'á':0x8F,'É':0x90,'é':0x91,'Í':0x92,'í':0x93,'Ó':0x94,'ó':0x95,'Ú':0x96,'ú':0x97,'Ñ':0x98,'ñ':0x99,'¡':0x9A,'¿':0x9B};
      function encodedFontCodes(item,text) {
        const codes=[];
        for(const character of text) {
          const code=character.codePointAt(0);
          if(code<0x80) codes.push(code);
          else if(character==='℃') codes.push(0x8B,0x43);
          else if(character==='ß') codes.push(0x73,0x73);
          else if(extendedGlyphCodes[character]!==undefined) codes.push(extendedGlyphCodes[character]);
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
        applyPreviewClip(node,item,origin,sx,sy);
      }
      function applyPreviewClip(node,item,origin,sx,sy) {
        // Same rule as the panel: a box set to its own panel is clipped there.
        // Clipping the node also keeps its hidden part from catching clicks
        // meant for boxes on the neighbouring panel; unclipped sides keep room
        // for the outline and badges.
        if(item.scrollArea!==0) { node.style.clipPath=''; return; }
        const canvas=node.querySelector('canvas');
        const left=(item.panel%state.columns)*PW, top=Math.floor(item.panel/state.columns)*PH;
        const width=parseFloat(canvas.style.width)||0, height=parseFloat(canvas.style.height)||0;
        const inset=value => value>0?value+'px':'-20px';
        const insetTop=(top-origin.y)*sy, insetLeft=(left-origin.x)*sx;
        const insetRight=width-(left+PW-origin.x)*sx, insetBottom=height-(top+PH-origin.y)*sy;
        node.style.clipPath=`inset(${inset(insetTop)} ${inset(insetRight)} ${inset(insetBottom)} ${inset(insetLeft)})`;
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
        drawPreviewCanvas(node.querySelector('canvas'),item,sx,sy);
        positionPreviewNode(node,item,sx,sy);
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
            ? boxNames[item.kind]+' • '+(item.valueSource===0?'Modbus':t('Dijital giriş'))
            : item.cycleEnabled
            ? t('Döngü: {types}',{types:cycleTypes(item).map(type=>t(typeNames[type])).join(' → ')})
            : t(typeNames[item.type])) + ' • '+fontName(item)+' • P' + (item.panel+1);
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
        $('panelSelection').innerHTML = '<strong>' + t('Panel {number}',{number:state.selectedPanel+1}) + '</strong> ' + t('seçili') + ' • ' + count + '/2 ' + t('öğe');
        addTextButton.disabled = count >= state.maxPerPanel;
        addValueButton.disabled = count >= state.maxPerPanel;
        $('panelSize').textContent = t('{columns} × {rows} panel',{columns:state.columns,rows:state.rows});
        $('logicalSize').textContent = t('{width} × {height} piksel',{width:width(),height:height()});
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
        $('coordX').textContent=x; $('coordY').textContent=y; $('itemPanel').textContent=t('Panel {number}',{number:target+1});
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
          counterDirection:0, temperatureFormat:0, font:state.defaultFont,
          scrollArea:0, decimals:0, suffix:'', currentRaw:0,
          countdownSeconds:defaultCountdownSeconds, countdownAlarm:defaultCountdownAlarm };
        state.items.push(item); state.selectedId=item.id;
        renderStage(); syncEditor(); setStatus('Yeni {box} eklendi.','',{box:boxNames[kind]});
      }
      function deleteItem() {
        const item=selectedItem(); if (!item) return;
        const name=boxNames[item.kind];
        state.items=state.items.filter(value => value.id !== item.id); state.selectedId=null;
        renderStage(); syncEditor(); setStatus('{box} silindi. Değişikliği uygulayabilirsiniz.','',{box:name});
      }

      function syncEditor() {
        const item=selectedItem(), editor=$('editor'), placeholder=$('editorPlaceholder');
        editor.hidden=!item; placeholder.hidden=!!item;
        if (!item) return;
        const isValue=item.kind===1;
        $('editor-title').textContent=t('{box} Ayarları',{box:boxNames[item.kind]});
        $('textBoxOptions').hidden=isValue;
        $('valueBoxOptions').hidden=!isValue;
        $('deleteItem').textContent=t('{box} Sil',{box:boxNames[item.kind]});
        document.querySelectorAll('[data-type]').forEach(button => {
          const type=Number(button.dataset.type);
          const active=type===item.type;
          button.classList.toggle('active',active); button.setAttribute('aria-pressed',String(active));
          button.disabled=item.cycleEnabled || (type===4 && !state.sensor.humidity && !active);
        });
        $('humidityNote').hidden=state.sensor.humidity;
        $('humidityNote').textContent=t('Seçili sensör ({sensor}) nem ölçmez. Sensör, Ayarlar → Sıcaklık Sensörü bölümünden değiştirilebilir.',{sensor:state.sensor.name});
        const cycleUsesText=item.cycleEnabled && (item.cycleMask&1)!==0;
        const isCountdown=!item.cycleEnabled && item.type===countdownType;
        $('textValue').value=item.text; $('textValue').disabled=item.cycleEnabled?!cycleUsesText:item.type!==0;
        $('cycleEnabled').checked=item.cycleEnabled;
        $('cycleEnabled').disabled=isCountdown;
        $('cycleCountdownNote').hidden=!isCountdown;
        $('countdownOptions').hidden=!isCountdown;
        const duration=Number(item.countdownSeconds)||0;
        $('durationHours').value=Math.floor(duration/3600);
        $('durationMinutes').value=Math.floor(duration/60)%60;
        $('durationSeconds').value=duration%60;
        $('countdownAlarm').value=item.countdownAlarm;
        $('cycleOptions').hidden=!item.cycleEnabled;
        document.querySelectorAll('[data-cycle-type]').forEach(input => {
          input.checked=(item.cycleMask&(1<<Number(input.dataset.cycleType)))!==0;
          input.disabled=Number(input.dataset.cycleType)===4 && !state.sensor.humidity && !input.checked;
        });
        $('cycleInterval').value=item.cycleInterval;
        const usesTemperature=item.cycleEnabled?(item.cycleMask&8)!==0:item.type===3;
        $('temperatureFormatOptions').hidden=!usesTemperature && !usesHumidity(item);
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
        $('valueDecimals').value=item.decimals;
        $('valueSuffix').value=item.suffix;
        $('itemFont').value=item.font;
        $('itemRotation').value=item.rotation;
        $('itemScrollArea').value=item.scrollArea;
        $('itemColor').value=itemColor(item); $('colorCode').textContent=itemColor(item).toUpperCase();
        $('scrollEnabled').checked=item.scrolling;
        $('scrollDirection').value=item.direction; $('scrollDirection').disabled=!item.scrolling;
        $('scrollSpeed').value=item.speed; $('scrollSpeed').disabled=!item.scrolling;
        $('speedValue').textContent=t('{speed} px/sn',{speed:item.speed});
        $('coordX').textContent=item.x; $('coordY').textContent=item.y;
        $('itemPanel').textContent=t('Panel {number}',{number:item.panel+1});
      }

      document.querySelectorAll('[data-type]').forEach(button => button.addEventListener('click',() => {
        const item=selectedItem(); if (!item) return;
        item.type=Number(button.dataset.type);
        // A countdown never cycles; its mask stays at the plain text bit.
        item.cycleMask=item.type===countdownType?1:1<<item.type;
        if(item.type===countdownType) item.cycleEnabled=false;
        item._cycleStartedAt=Date.now(); syncEditor(); renderStage(); setStatus('Veri türü değişti.');
      }));
      function updateCountdownDuration() {
        const item=selectedItem(); if(!item||item.kind!==0) return;
        const part=(id,max)=>Math.max(0,Math.min(max,Math.trunc(Number($(id).value))||0));
        item.countdownSeconds=part('durationHours',23)*3600+part('durationMinutes',59)*60+part('durationSeconds',59);
        renderStage();
      }
      ['durationHours','durationMinutes','durationSeconds'].forEach(id => {
        $(id).addEventListener('input',updateCountdownDuration);
        $(id).addEventListener('change',() => { updateCountdownDuration(); syncEditor(); setStatus('Geri sayım süresi değişti.'); });
      });
      $('countdownAlarm').addEventListener('input',event => {
        const item=selectedItem(); if(!item||item.kind!==0) return;
        item.countdownAlarm=Number(event.target.value);
      });
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
        item.valueSource=Number(button.dataset.valueSource); item.currentRaw=Number(item.initialValue)||0;
        syncEditor(); renderStage(); setStatus('ValueBox veri kaynağı değişti.');
      }));
      $('initialValue').addEventListener('input',event => {
        const item=selectedItem(); if(!item||item.kind!==1) return;
        item.initialValue=event.target.value; item.currentRaw=Number(item.initialValue)||0; renderStage();
      });
      $('registerAddress').addEventListener('input',event => { const item=selectedItem(); if(item&&item.kind===1) item.registerAddress=Number(event.target.value); });
      $('valueType').addEventListener('change',event => {
        const item=selectedItem(); if(!item||item.kind!==1) return;
        item.valueType=Number(event.target.value); item.currentRaw=Number(item.initialValue)||0; syncEditor(); renderStage();
      });
      $('valueDecimals').addEventListener('change',event => {
        const item=selectedItem(); if(!item||item.kind!==1) return;
        item.decimals=Number(event.target.value); renderStage(); setStatus('Ondalık basamak sayısı değişti.');
      });
      $('valueSuffix').addEventListener('input',event => { const item=selectedItem(); if(item&&item.kind===1){ item.suffix=event.target.value; renderStage(); } });
      $('itemScrollArea').addEventListener('change',event => {
        const item=selectedItem(); if(!item) return;
        item.scrollArea=Number(event.target.value)===1?1:0; renderStage(); setStatus('Gösterim alanı değişti.');
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
      $('scrollSpeed').addEventListener('input',event => { const item=selectedItem(); if(item){ item.speed=Number(event.target.value); $('speedValue').textContent=t('{speed} px/sn',{speed:item.speed}); } });
      addTextButton.addEventListener('click',()=>addItem(0));
      addValueButton.addEventListener('click',()=>addItem(1));
      $('deleteItem').addEventListener('click',deleteItem);

      async function request(url,options) {
        const response=await fetch(url,{cache:'no-store',...options});
        const data=await response.json().catch(()=>({ok:false,message:'Geçersiz cihaz yanıtı.'}));
        if(!response.ok || data.ok===false) throw new Error(I18n.server(data.message||data.error,'İşlem başarısız.'));
        return data;
      }
      async function loadContent() {
        try {
          const data=await request('/api/content');
          state.rows=data.panel.rows; state.columns=data.panel.columns; state.maxPerPanel=data.panel.maxPerPanel;
          if(data.sensor) state.sensor={type:Number(data.sensor.type), name:String(data.sensor.name), humidity:Boolean(data.sensor.humidity), missingText:String(data.sensor.missingText)};
          state.fonts=Array.isArray(data.fonts)?data.fonts:[];
          state.defaultFont=Number.isInteger(Number(data.defaultFont))?Number(data.defaultFont):2;
          const fontSelect=$('itemFont'); fontSelect.replaceChildren();
          state.fonts.forEach((name,id)=>{
            const option=document.createElement('option'); option.value=String(id); option.textContent=name; fontSelect.appendChild(option);
          });
          state.items=data.items.map(item=>{
            const kind=Number(item.kind)||0;
            return {...item,kind,rotation:item.rotation||0,
              cycleEnabled:Boolean(item.cycleEnabled),cycleMask:Number(item.cycleMask)||(1<<item.type),
              cycleInterval:Number(item.cycleInterval)||5,initialValue:Number(item.initialValue)||0,
              valueSource:Number(item.valueSource)||0,registerAddress:Number(item.registerAddress)||0,
              valueType:Number(item.valueType)||0,counterDirection:Number(item.counterDirection)||0,
              temperatureFormat:Number(item.temperatureFormat)===1?1:0,
              font:Number.isInteger(Number(item.font))?Number(item.font):state.defaultFont,
              scrollArea:Number(item.scrollArea)===1?1:0,decimals:Number(item.decimals)||0,
              // ValueBoxes store their unit suffix in the text field.
              text:kind===1?'YAZI':String(item.text||''),suffix:kind===1?String(item.text||''):'',
              currentRaw:item.currentRaw===null||item.currentRaw===undefined?null:Number(item.currentRaw),
              countdownSeconds:Number(item.countdownSeconds)||defaultCountdownSeconds,
              countdownAlarm:Number.isFinite(Number(item.countdownAlarm))?Number(item.countdownAlarm):defaultCountdownAlarm,
              _cycleStartedAt:Date.now(),id:state.nextId++};
          });
          const requiredFonts=new Set([state.defaultFont,...state.items.map(item=>item.font)]);
          for(const fontId of requiredFonts) await loadFontData(fontId);
          state.selectedPanel=0; state.selectedId=state.items.length?state.items[0].id:null;
          renderStage(); syncEditor(); saveButton.disabled=false;
          if(!state.sensor.humidity && state.items.some(usesHumidity)) setStatus('Seçili sensör ({sensor}) nem ölçmez; nem gösteren kutuları değiştirin.','error',{sensor:state.sensor.name});
          else setStatus('Panel içeriği hazır.','ok');
        } catch(error) { setStatus(error.message,'error'); }
      }
      async function loadTemperature() {
        try {
          const data=await request('/api/temperature');
          state.temperatureValue=data.ready && Number.isFinite(data.temperature)
            ? Number(data.temperature)
            : null;
          state.humidityValue=data.ready && Number.isFinite(data.humidity)
            ? Number(data.humidity)
            : null;
        } catch(error) { state.temperatureValue=null; state.humidityValue=null; }
      }
      async function saveContent() {
        const registerRanges=[];
        for (const item of state.items) {
          if(!Number.isInteger(item.font)||item.font<0||item.font>=state.fonts.length){ selectItem(item.id); setStatus('Yazı tipi seçimi geçersiz.','error'); return; }
          if(usesHumidity(item) && !state.sensor.humidity){ selectItem(item.id); setStatus('Seçili sensör ({sensor}) nem ölçmez; nem gösteren kutuları değiştirin.','error',{sensor:state.sensor.name}); return; }
          if(item.kind===0){
            const textRequired=(!item.cycleEnabled&&item.type===0)||(item.cycleEnabled&&(item.cycleMask&1)!==0);
            if (textRequired && !item.text.trim()) { selectItem(item.id); setStatus('Yazı içeriği boş bırakılamaz.','error'); return; }
            if (utf8Length(item.text)>47) { selectItem(item.id); setStatus('Metin en fazla 47 bayt olabilir.','error'); return; }
            if (item.cycleEnabled && !item.cycleMask) { selectItem(item.id); setStatus('Döngü için en az bir bilgi seçilmelidir.','error'); return; }
            if (item.cycleEnabled && (!Number.isInteger(item.cycleInterval)||item.cycleInterval<1||item.cycleInterval>3600)) { selectItem(item.id); setStatus('Değişim sıklığı 1-3600 saniye olmalıdır.','error'); return; }
            if (!item.cycleEnabled && item.type===countdownType) {
              if (!Number.isInteger(item.countdownSeconds)||item.countdownSeconds<1||item.countdownSeconds>maximumCountdownSeconds) { selectItem(item.id); setStatus('Geri sayım süresi 1 saniye ile 23:59:59 arasında olmalıdır.','error'); return; }
              if (!Number.isInteger(item.countdownAlarm)||item.countdownAlarm<0||item.countdownAlarm>3600) { selectItem(item.id); setStatus('Geri sayım alarm süresi 0-3600 saniye olmalıdır.','error'); return; }
            }
            continue;
          }

          const initial=Number(item.initialValue);
          if(!Number.isFinite(initial)){ selectItem(item.id); setStatus('ValueBox başlangıç değeri geçersiz.','error'); return; }
          if(utf8Length(item.suffix)>15){ selectItem(item.id); setStatus('ValueBox birimi en fazla 15 bayt olabilir.','error'); return; }
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
        // Field order is documented at parseContentPayload() in PortalApplication.cpp.
        const records=state.items.map(item => [item.panel,item.type,item.rotation,item.x,item.y,
          itemColor(item).slice(1).toUpperCase(),item.scrolling?1:0,item.direction,item.speed,
          encodeURIComponent((item.kind===1?item.suffix:item.text)||''),item.cycleEnabled?1:0,item.cycleMask,item.cycleInterval,
          item.kind,Number(item.initialValue)||0,item.valueSource,item.registerAddress,item.valueType,
          item.counterDirection,item.temperatureFormat,item.font,item.scrollArea,item.kind===1?item.decimals:0,
          item.kind===0?item.countdownSeconds:defaultCountdownSeconds,item.kind===0?item.countdownAlarm:defaultCountdownAlarm].join('|')).join('\n');
        saveButton.disabled=true; setStatus('Panele uygulanıyor…');
        try { await request('/api/content',{method:'POST',headers:{'Content-Type':'text/plain;charset=utf-8'},body:records}); setStatus('İçerik panele uygulandı ve flash belleğe kaydedildi.','ok'); }
        catch(error) { setStatus(error.message,'error'); }
        finally { saveButton.disabled=false; }
        loadCountdown();
      }

      let countdownTimer=0, countdownBusy=false;
      function renderCountdown() {
        const session=state.countdown, card=$('countdownCard');
        card.hidden=!session.items.length;
        if(!session.items.length) return;
        const elapsed=countdownElapsed(), times=$('countdownTimes');
        times.replaceChildren();
        session.items.forEach(item => {
          const box=document.createElement('div'); box.className='countdown-time';
          const label=document.createElement('span');
          label.textContent=t('Panel {number}',{number:item.panel+1})+' • '+formatCountdown(item.duration,0);
          const value=document.createElement('b'); value.textContent=formatCountdown(item.duration,elapsed);
          box.append(label,value); times.appendChild(box);
        });
        const pill=$('countdownState');
        pill.textContent=t(countdownStateNames[session.state]||countdownStateNames.idle);
        pill.className='panel-pill '+session.state;
        $('countdownAlarmActive').hidden=!session.alarmActive;
        $('countdownStart').textContent=session.state==='paused'?t('Devam Et'):session.state==='finished'?t('Yeniden Başlat'):t('Başlat');
        $('countdownStart').disabled=countdownBusy||session.state==='running';
        $('countdownPause').disabled=countdownBusy||(session.state!=='running'&&!session.alarmActive);
        $('countdownReset').disabled=countdownBusy||(session.state==='idle'&&!session.alarmActive);
      }
      function applyCountdown(data) {
        state.countdown={state:String(data.state||'idle'),elapsedMs:Number(data.elapsedMs)||0,receivedAt:Date.now(),
          alarmActive:Boolean(data.alarmActive),
          items:Array.isArray(data.items)?data.items.map(item=>({panel:Number(item.panel)||0,duration:Number(item.duration)||0})):[]};
        renderCountdown();
      }
      async function loadCountdown() {
        clearTimeout(countdownTimer);
        try { applyCountdown(await request('/api/countdown')); }
        catch(error) { /* The next poll retries. */ }
        // Poll only while the applied design has a countdown.
        if(state.countdown.items.length) countdownTimer=setTimeout(loadCountdown,1000);
      }
      async function controlCountdown(action) {
        if(action==='reset' && (state.countdown.state==='running'||state.countdown.state==='paused')
            && !confirm(t('Geri sayım sıfırlansın mı? Geçen süre silinir.'))) return;
        countdownBusy=true; renderCountdown();
        try {
          const data=await request('/api/countdown',{method:'POST',headers:{'Content-Type':'application/x-www-form-urlencoded'},body:'action='+action});
          applyCountdown(data); setStatus(data.message,'ok',{},countdownStatus);
        } catch(error) { setStatus(error.message,'error',{},countdownStatus); }
        finally { countdownBusy=false; renderCountdown(); loadCountdown(); }
      }
      $('countdownStart').addEventListener('click',()=>controlCountdown('start'));
      $('countdownPause').addEventListener('click',()=>controlCountdown('pause'));
      $('countdownReset').addEventListener('click',()=>controlCountdown('reset'));
      saveButton.addEventListener('click',saveContent);
      window.addEventListener('resize',renderStage);
      I18n.onChange(() => {
        renderStage();
        syncEditor();
        renderCountdown();
        retranslateStatus(status);
        retranslateStatus(countdownStatus);
      });
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
      // Between polls the remaining time is estimated locally.
      setInterval(() => { if(state.countdown.state==='running') renderCountdown(); },250);
      loadContent();
      loadCountdown();
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
    .topbar .language-select {
      width: auto; min-width: 116px; height: 44px; margin-inline-start: auto; padding: 0 34px 0 12px;
      color: #dce9ff; border: 1px solid rgba(255,255,255,.13); border-radius: 14px;
      background: #102139; cursor: pointer;
    }
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
    .section-form { margin-top: 26px; padding-top: 24px; border-top: 1px solid rgba(255,255,255,.08); }
    [hidden] { display: none !important; }
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
    .mode-choices { display: grid; grid-template-columns: repeat(2,minmax(0,1fr)); gap: 10px; margin-bottom: 20px; }
    .mode-choice { position: relative; }
    .mode-choice input { position: absolute; opacity: 0; pointer-events: none; }
    .mode-choice > span {
      display: grid; gap: 4px; min-height: 66px; align-content: center; padding: 11px 14px;
      color: #9fb2cc; cursor: pointer; border: 1px solid rgba(255,255,255,.12);
      border-radius: 14px; background: #0a1829;
    }
    .mode-choice strong { font-size: 14px; }
    .mode-choice small { color: #8195b2; font-size: 12px; line-height: 1.35; }
    .mode-choice input:checked + span {
      color: #fff; border-color: #398cff; background: rgba(42,126,243,.19);
      box-shadow: 0 0 0 2px rgba(57,140,255,.12);
    }
    .mode-choice input:focus-visible + span { outline: 2px solid #4b98ff; outline-offset: 2px; }
    .subsection-title { margin: 26px 0 6px; color: #e7f0fc; font-size: 16px; }
    .subsection-title + .sleep-hint { margin-bottom: 14px; }
    .lesson-fields { display: grid; grid-template-columns: repeat(2,minmax(0,1fr)); gap: 12px; }
    .lesson-actions { display: flex; flex-wrap: wrap; gap: 8px; margin-top: 11px; }
    .link-button {
      min-height: 34px; padding: 6px 11px; color: #a9cdff; cursor: pointer;
      border: 1px dashed rgba(80,151,255,.45); border-radius: 9px; background: transparent; font-weight: 800;
    }
    .lesson-info { margin: 10px 0 0; color: #91a5c2; font-size: 12px; line-height: 1.45; }
    .lesson-info:empty { display: none; }
    .lesson-info.error { color: #ff9aa6; }
    .sleep-toggle .toggle-note { display: block; margin-top: 3px; color: #ffd27a; font-size: 12px; font-weight: 700; }
    .holiday-list {
      grid-column: 1 / -1; display: grid; gap: 7px; margin: 0; padding: 12px 14px; list-style: none;
      color: #b8c9df; font-size: 12px; border: 1px solid rgba(255,255,255,.10); border-radius: 12px; background: #0a1829;
    }
    .holiday-list li { display: flex; justify-content: space-between; gap: 12px; }
    .holiday-list time { flex: 0 0 auto; color: #e7f0fc; font-weight: 800; }
    .holiday-list .holiday-title { color: #8195b2; font-weight: 800; }
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
    .tab-badge {
      display: inline-block; margin-inline-start: 7px; padding: 2px 7px; color: #06101d;
      font-size: 11px; font-weight: 900; vertical-align: middle; border-radius: 999px; background: #ffd27a;
    }
    .update-offer {
      display: grid; gap: 6px; margin: 0 0 14px; padding: 13px 15px;
      border: 1px solid rgba(255,190,70,.35); border-radius: 13px; background: rgba(255,170,30,.10);
    }
    .update-offer strong { color: #ffe7b8; font-size: 14px; }
    .online-update { margin-bottom: 6px; }
    .online-update .firmware-progress { margin: 0 0 14px; }
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
    html[dir="rtl"] body { direction: rtl; }
    html[dir="rtl"] .back { transform: scaleX(-1); }
    html[dir="rtl"] input[type="number"],
    html[dir="rtl"] input[type="time"],
    html[dir="rtl"] input[type="date"],
    html[dir="rtl"] code,
    html[dir="rtl"] .holiday-list time,
    html[dir="rtl"] .connection-card svg { direction: ltr; }
    @media (max-width: 650px) {
      .shell { padding: 16px 12px 34px; }
      .topbar { flex-wrap: wrap; }
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
      .lesson-fields { grid-template-columns: 1fr; }
      .holiday-list li { flex-direction: column; gap: 2px; }
    }
  </style>
</head>
<body>
  <div class="shell">
    <div class="topbar">
      <a class="back" href="/" aria-label="Geri">&#8249;</a>
      <div><h1>Ayarlar</h1><p class="subtitle">STEP-RGB panel kontrolü</p></div>
      <select class="language-select" id="languageSelect" aria-label="Dil" title="Dil">
        <option value="tr">Türkçe</option><option value="en">English</option><option value="de">Deutsch</option><option value="es">Español</option><option value="ar">العربية</option>
      </select>
    </div>

    <nav class="tabs" aria-label="Ayar grupları">
      <button class="tab active" data-target="panel">Panel Ayarları</button>
      <button class="tab" data-target="clock">Saat ve Tarih Ayarları</button>
      <button class="tab" data-target="sensor">Sıcaklık Sensörü</button>
      <button class="tab" data-target="alarm">Alarm Ayarları</button>
      <button class="tab" data-target="communication">Haberleşme Ayarları</button>
      <button class="tab" data-target="wifi">Wi-Fi Ayarları</button>
      <button class="tab" data-target="firmware">Yazılım Güncelleme<span class="tab-badge" id="firmware-tab-badge" hidden>Yeni</span></button>
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
            <p class="sleep-hint">Yalnızca parlaklık değiştiyse ayar yeniden başlatmadan hemen uygulanır.</p>
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
          <p class="sta-warning" id="rtc-missing" role="note" hidden><span aria-hidden="true">&#9888;</span><span>RTC bulunamadı. DS3231 bağlantısını kontrol edin; saat ve tarih ayarlanamaz.</span></p>
          <div class="field"><label for="date">Tarih</label><input id="date" name="date" type="date" required></div>
          <div class="field"><label for="time">Saat</label><input id="time" name="time" type="time" step="1" required></div>
          <p class="sleep-hint">Alanlar cihaz saatiyle birlikte ilerler. Elle girdiğiniz değer de kaydedene kadar saymaya devam eder.</p>
        </div>
        <div class="actions">
          <button class="button secondary" id="browser-time" type="button">Telefon/Bilgisayar Saatini Kullan</button>
          <button class="button" id="clock-submit" type="submit">Saat ve Tarihi Kaydet</button>
        </div>
      </form>

      <form id="time-sync-form" class="section-form">
        <div class="grid">
          <fieldset>
            <legend>İnternet saati (NTP)</legend>
            <label class="sleep-toggle" for="ntpEnabled">
              <input id="ntpEnabled" type="checkbox" value="1">
              STA ağına bağlıyken saati otomatik eşitle
            </label>
            <div class="field"><label for="ntpServer">NTP sunucusu</label><input id="ntpServer" name="ntpServer" maxlength="63" required></div>
            <div class="field">
              <label for="timeZonePreset">Saat dilimi</label>
              <select id="timeZonePreset">
                <option value="&lt;+03&gt;-3">UTC+3 — Türkiye, Suudi Arabistan</option>
                <option value="UTC0">UTC</option>
                <option value="GMT0BST,M3.5.0/1,M10.5.0">Birleşik Krallık (UTC+0, yaz saati)</option>
                <option value="CET-1CEST,M3.5.0,M10.5.0/3">Orta Avrupa — Almanya, İspanya (UTC+1, yaz saati)</option>
                <option value="EET-2EEST,M3.5.0/3,M10.5.0/4">Doğu Avrupa — Yunanistan (UTC+2, yaz saati)</option>
                <option value="MSK-3">Moskova (UTC+3)</option>
                <option value="&lt;+04&gt;-4">Körfez — BAE (UTC+4)</option>
                <option value="EST5EDT,M3.2.0,M11.1.0">ABD Doğu (UTC−5, yaz saati)</option>
                <option value="custom">Özel POSIX TZ değeri</option>
              </select>
            </div>
            <div class="field full" id="custom-time-zone" hidden><label for="timeZone">POSIX TZ değeri</label><input id="timeZone" maxlength="63" placeholder="CET-1CEST,M3.5.0,M10.5.0/3"></div>
            <p class="sleep-hint" id="ntp-status">—</p>
            <p class="sleep-hint">Eşitlenen yerel saat RTC'ye yazılır; yaz saati geçişleri seçilen saat dilimine göre uygulanır. NTP açıkken elle girilen saat bir sonraki eşitlemede düzeltilir.</p>
          </fieldset>
        </div>
        <div class="actions"><button class="button" type="submit">NTP Ayarlarını Kaydet</button></div>
      </form>

      <form id="sleep-form" class="section-form">
        <div class="grid">
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
        <div class="actions"><button class="button" type="submit">Uyku Ayarlarını Kaydet</button></div>
      </form>
    </section>

    <section class="panel" id="sensor-section">
      <h2>Sıcaklık Sensörü</h2>
      <p class="note">Karta takılı sensör modelini seçin. Ayarlar kaydedildiğinde cihaz yeniden başlatılmadan uygulanır. Seçili sensör bulunamazsa panelde TMP?, SHT? veya HDC? gösterilir.</p>
      <div class="wifi-connection" id="sensor-status" data-state="waiting" role="status" aria-live="polite">
        <span class="wifi-dot" aria-hidden="true"></span>
        <span class="wifi-copy">
          <strong id="sensor-status-label">Sensör durumu alınıyor</strong>
          <span id="sensor-status-detail">Lütfen bekleyin…</span>
        </span>
        <code class="wifi-ip" id="sensor-reading" hidden></code>
      </div>
      <form id="sensor-form">
        <div class="grid">
          <div class="field full">
            <label for="sensorType">Sensör modeli</label>
            <select id="sensorType" name="type">
              <option value="1">TMP112 — yalnızca sıcaklık</option>
              <option value="0">SHT40 — sıcaklık ve nem</option>
              <option value="2">HDC2022 — sıcaklık ve nem</option>
            </select>
          </div>
          <div class="field"><label for="sensorInterval">Ölçüm aralığı (saniye)</label><input id="sensorInterval" name="interval" type="number" min="1" max="60" step="1" inputmode="numeric" required></div>
          <div class="field"><label for="temperatureOffset">Sıcaklık düzeltmesi (°C)</label><input id="temperatureOffset" type="number" min="-10" max="10" step="0.1" inputmode="decimal" required></div>
          <div class="field" id="humidity-offset-field"><label for="humidityOffset">Nem düzeltmesi (%RH)</label><input id="humidityOffset" type="number" min="-20" max="20" step="0.1" inputmode="decimal" required></div>
          <p class="sleep-hint">Düzeltme değerleri ölçülen değere eklenir. Sensörü güvenilir bir referans ölçerle karşılaştırarak ayarlayın; sensör modeli değiştiğinde düzeltmeyi yeniden kontrol edin.</p>
          <fieldset data-sensor-options="0">
            <legend>SHT40 ayarları</legend>
            <div class="field"><label for="sht40Address">I2C adresi</label><select id="sht40Address" name="sht40Address"><option value="68">0x44 — SHT40-AD1B</option><option value="69">0x45 — SHT40-BD1B</option></select></div>
            <div class="field"><label for="sht40Precision">Ölçüm hassasiyeti</label><select id="sht40Precision" name="sht40Precision"><option value="0">Yüksek — en fazla 8.2 ms</option><option value="1">Orta — en fazla 4.5 ms</option><option value="2">Düşük — en fazla 1.7 ms</option></select></div>
            <p class="sleep-hint">Yüksek hassasiyet en düşük ölçüm gürültüsünü verir; düşük hassasiyet ölçümü kısaltır ve daha az enerji harcar. Adres, ürün kodundaki AD1B/BD1B sürümüne göre sabittir.</p>
          </fieldset>
          <fieldset data-sensor-options="1">
            <legend>TMP112 ayarları</legend>
            <div class="field">
              <label for="tmp112Address">I2C adresi</label>
              <select id="tmp112Address" name="tmp112Address">
                <option value="72">0x48 — ADD0 → GND</option>
                <option value="73">0x49 — ADD0 → V+</option>
                <option value="74">0x4A — ADD0 → SDA</option>
                <option value="75">0x4B — ADD0 → SCL</option>
                <option value="64">0x40 — TMP112D X2SON, ADD0 → GND</option>
                <option value="65">0x41 — TMP112D X2SON, ADD0 → V+</option>
                <option value="66">0x42 — TMP112D X2SON, ADD0 → SDA</option>
                <option value="67">0x43 — TMP112D X2SON, ADD0 → SCL</option>
              </select>
            </div>
            <div class="field">
              <label for="tmp112ConversionRate">Dönüşüm hızı</label>
              <select id="tmp112ConversionRate" name="tmp112ConversionRate">
                <option value="0">0.25 Hz</option>
                <option value="1">1 Hz</option>
                <option value="2">4 Hz (varsayılan)</option>
                <option value="3">8 Hz</option>
              </select>
            </div>
            <label class="sleep-toggle" for="tmp112ExtendedMode">
              <input id="tmp112ExtendedMode" type="checkbox" value="1">
              Genişletilmiş ölçüm aralığı (13 bit, 150 °C'ye kadar)
            </label>
            <p class="sleep-hint">TMP112 yalnızca sıcaklık ölçer; nem gösteren kutular bu sensörle kullanılamaz. Sensör sürekli ölçüm yapar; düşük dönüşüm hızı akım tüketimini azaltır.</p>
          </fieldset>
          <fieldset data-sensor-options="2">
            <legend>HDC2022 ayarları</legend>
            <div class="field full"><label for="hdc2022Address">I2C adresi</label><select id="hdc2022Address" name="hdc2022Address"><option value="64">0x40 — ADDR → GND</option><option value="65">0x41 — ADDR → VDD</option></select></div>
            <div class="field"><label for="hdc2022TemperatureResolution">Sıcaklık çözünürlüğü</label><select id="hdc2022TemperatureResolution" name="hdc2022TemperatureResolution"><option value="0">14 bit — 594 µs</option><option value="1">11 bit — 336 µs</option><option value="2">9 bit — 208 µs</option></select></div>
            <div class="field"><label for="hdc2022HumidityResolution">Nem çözünürlüğü</label><select id="hdc2022HumidityResolution" name="hdc2022HumidityResolution"><option value="0">14 bit — 640 µs</option><option value="1">11 bit — 383 µs</option><option value="2">9 bit — 254 µs</option></select></div>
            <p class="sleep-hint">ADDR pini boşta bırakılırsa adres 0x40 olur; gürültülü ortamlarda pini GND veya VDD'ye bağlayın. Ölçüm her aralıkta tek seferlik tetiklenir.</p>
          </fieldset>
        </div>
        <div class="actions"><button class="button" type="submit">Sensör Ayarlarını Kaydet</button></div>
      </form>
    </section>

    <section class="panel" id="alarm-section">
      <h2>Alarm Ayarları</h2>
      <p class="note">IO42 alarm rölesi manuel alarmlarla veya okul saati programıyla çalışır. Aynı anda yalnızca biri etkindir; diğerinin ayarları saklanır.</p>
      <form id="alarm-form">
        <div class="mode-choices" role="radiogroup" aria-label="Alarm modu">
          <label class="mode-choice"><input type="radio" name="alarmMode" value="0" checked><span><strong>Manuel alarm</strong><small>Belirlediğiniz saatlerde çalan en fazla 10 alarm</small></span></label>
          <label class="mode-choice"><input type="radio" name="alarmMode" value="1"><span><strong>Okul saati</strong><small>Ders ve teneffüs zilleri, resmi tatiller ve tatil dönemleri</small></span></label>
        </div>

        <div id="manual-alarm-panel">
          <p class="note">Alarm tetiklendiğinde IO42 rölesi belirlenen süre boyunca açık kalır. En fazla 10 alarm oluşturabilirsiniz.</p>
          <div class="alarm-summary"><strong id="alarm-count">0 / 10 alarm</strong><span>Süre: 1–3600 saniye</span></div>
          <p class="alarm-empty" id="alarm-empty">Henüz alarm oluşturulmadı.</p>
          <div class="alarm-list" id="alarm-list"></div>
          <div class="actions"><button class="button secondary" id="add-alarm" type="button">Yeni Alarm Ekle</button></div>
        </div>

        <div id="school-bell-panel" hidden>
          <div class="wifi-connection" id="school-status" data-state="waiting" role="status" aria-live="polite">
            <span class="wifi-dot" aria-hidden="true"></span>
            <span class="wifi-copy">
              <strong id="school-status-label">Okul saati durumu alınıyor</strong>
              <span id="school-status-detail">Lütfen bekleyin…</span>
            </span>
          </div>
          <div class="grid">
            <fieldset>
              <legend>Genel ayarlar</legend>
              <div class="field"><label for="schoolDayMode">Çalışma günleri</label><select id="schoolDayMode"><option value="0">Haftanın her günü</option><option value="1">Sadece hafta içi</option><option value="2">İstediğim günler</option></select></div>
              <div class="field"><label for="schoolDuration">Zil süresi (saniye)</label><input id="schoolDuration" type="number" min="1" max="3600" step="1" inputmode="numeric"></div>
              <div class="alarm-custom-days" id="school-custom-days" hidden>
                <p class="sleep-hint">Okul zilinin çalışacağı günleri seçin.</p>
                <div class="alarm-day-grid">
                  <label class="alarm-day-choice"><input type="checkbox" data-school-day="0"><span>Pazartesi</span></label>
                  <label class="alarm-day-choice"><input type="checkbox" data-school-day="1"><span>Salı</span></label>
                  <label class="alarm-day-choice"><input type="checkbox" data-school-day="2"><span>Çarşamba</span></label>
                  <label class="alarm-day-choice"><input type="checkbox" data-school-day="3"><span>Perşembe</span></label>
                  <label class="alarm-day-choice"><input type="checkbox" data-school-day="4"><span>Cuma</span></label>
                  <label class="alarm-day-choice"><input type="checkbox" data-school-day="5"><span>Cumartesi</span></label>
                  <label class="alarm-day-choice"><input type="checkbox" data-school-day="6"><span>Pazar</span></label>
                </div>
              </div>
              <div class="field full"><label for="schoolStudentBell">Öğrenci zili</label><select id="schoolStudentBell"></select></div>
              <p class="sleep-hint">Öğrenci zili, öğrencileri sınıfa çağırmak için her dersten önce ek bir zil çalar; ders başlangıç zili öğretmen zili olarak çalmaya devam eder. Bütün ziller aynı zil süresini kullanır.</p>
              <label class="sleep-toggle" for="schoolSkipHolidays">
                <input id="schoolSkipHolidays" type="checkbox" value="1">
                <span>Resmi tatillerde zil çalmasın<small class="toggle-note">Yalnızca Türkiye'deki resmi tatiller</small></span>
              </label>
              <p class="sleep-hint">Ulusal bayramlarda, 1 Ocak, 1 Mayıs ve 15 Temmuz'da, Ramazan ve Kurban bayramlarında zil çalmaz. Bayram arifelerinde ve 28 Ekim'de saat 13:00'ten sonra zil çalmaz.</p>
              <ul class="holiday-list" id="holiday-list" hidden></ul>
              <p class="sleep-hint" id="holiday-range"></p>
              <label class="sleep-toggle" for="schoolRemembrance">
                <input id="schoolRemembrance" type="checkbox" value="1">
                <span>10 Kasım'da saat 09:05'te saygı duruşu zili çal</span>
              </label>
            </fieldset>
          </div>

          <h3 class="subsection-title">Ders programı</h3>
          <p class="sleep-hint">Dersleri saat sırasıyla ekleyin. Her dersin altına teneffüs zilini ekleyebilirsiniz; son dersin teneffüs zili okul çıkış zili olarak kullanılabilir. İkili öğretimde sabahçı ve öğlenci derslerini aynı listeye sırayla ekleyin.</p>
          <div class="alarm-summary"><strong id="lesson-count">0 / 20 ders</strong><span>Yeni ders, önceki teneffüsten 10 dakika sonra önerilir</span></div>
          <p class="alarm-empty" id="lesson-empty">Henüz ders eklenmedi.</p>
          <div class="alarm-list" id="lesson-list"></div>
          <div class="actions"><button class="button secondary" id="add-lesson" type="button">Ders Ekle</button></div>

          <h3 class="subsection-title">Tatil dönemleri</h3>
          <p class="sleep-hint">Yaz tatili, yarıyıl tatili, ara tatiller veya idari izin gibi okulun kapalı olduğu günleri ekleyin; bu günlerde zil çalmaz. Başlangıç ve bitiş günleri dahildir.</p>
          <div class="alarm-summary"><strong id="vacation-count">0 / 10 tatil dönemi</strong></div>
          <p class="alarm-empty" id="vacation-empty">Henüz tatil dönemi eklenmedi.</p>
          <div class="alarm-list" id="vacation-list"></div>
          <div class="actions"><button class="button secondary" id="add-vacation" type="button">Tatil Dönemi Ekle</button></div>
        </div>

        <div class="actions">
          <button class="button secondary" id="test-bell" type="button">Zili Test Et</button>
          <button class="button" type="submit">Alarm Ayarlarını Kaydet</button>
        </div>
      </form>
    </section>

    <section class="panel" id="communication-section">
      <h2>Haberleşme Ayarları</h2>
      <p class="note">RS485 Modbus RTU hattı RX IO39, TX IO40 ve DIR IO41 pinlerini kullanır. Değişiklikler kaydedildikten sonra cihaz otomatik yeniden başlar.</p>
      <form id="communication-form">
        <div class="grid">
          <div class="field">
            <label for="slaveAddress">Slave ID</label>
            <input id="slaveAddress" name="slaveAddress" type="number" min="1" max="247" step="1" inputmode="numeric" required>
          </div>
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
            <select id="handshake" name="handshake"><option value="1">RS485 DIR kontrolü (IO41)</option><option value="0">Yok / otomatik yön donanımı</option></select>
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
          <p class="sta-warning" id="default-password-warning" role="note" hidden><span aria-hidden="true">&#9888;</span><span>Varsayılan AP parolası kullanılıyor. Cihazı devreye almadan önce parolayı değiştirin.</span></p>
          <fieldset>
            <legend>Mevcut cihaz ağı (AP)</legend>
            <div class="field"><label for="apSsid">SSID</label><input id="apSsid" name="apSsid" maxlength="32" required></div>
            <div class="field"><label for="apPassword">Parola</label><input id="apPassword" name="apPassword" type="password" minlength="8" maxlength="63" autocomplete="new-password" placeholder="Kayıtlı parola korunur; değiştirmek için yazın"></div>
            <p class="sleep-hint">Kayıtlı parolalar güvenlik nedeniyle bu sayfada gösterilmez; alan boş bırakılırsa mevcut parola korunur.</p>
          </fieldset>
          <fieldset>
            <legend>Bağlanılacak ağ (STA)</legend>
            <p class="sta-warning" role="note"><span aria-hidden="true">&#9888;</span><span><strong>Yalnızca 2.4 GHz:</strong> ESP32-S3, 5 GHz Wi-Fi ağlarına bağlanamaz. STA için 2.4 GHz yayın yapan bir SSID seçin.</span></p>
            <div class="field"><label for="staSsid">SSID</label><input id="staSsid" name="staSsid" maxlength="32" placeholder="Boş bırakılırsa devre dışı"></div>
            <div class="field"><label for="staPassword">Parola</label><input id="staPassword" name="staPassword" type="password" maxlength="63" autocomplete="new-password" placeholder="Açık ağ için boş bırakılabilir"></div>
            <label class="sleep-toggle" for="staAuthentication">
              <input id="staAuthentication" type="checkbox" value="1">
              Bu ağdan erişimde kullanıcı adı ve parola iste
            </label>
            <p class="sleep-hint">Açıkken bu ağdan bağlanan tarayıcı giriş ister: kullanıcı adı admin, parola cihazın AP parolasıdır. Portal HTTPS kullanmadığı için Chrome bu girişte bağlantının gizli olmadığı uyarısını gösterir. Kapalıyken aynı ağdaki herkes ayarları değiştirebilir ve firmware yükleyebilir.</p>
          </fieldset>
        </div>
        <div class="actions"><button class="button" type="submit">Wi-Fi Ayarlarını Kaydet</button></div>
      </form>
    </section>

    <section class="panel" id="firmware-section">
      <h2>Yazılım Güncelleme</h2>
      <p class="note">Yeni sürümü GitHub üzerinden çevrimiçi kurabilir veya firmware.bin dosyasını bilgisayarınızdan ya da telefonunuzdan yükleyebilirsiniz. Kurulumdan sonra cihaz otomatik olarak yeniden başlar.</p>

      <div class="online-update">
        <h3 class="subsection-title">Çevrimiçi güncelleme</h3>
        <p class="sta-warning" id="online-update-rollback" role="alert" hidden></p>
        <p class="sta-warning" id="online-update-pending" role="status" hidden>Yeni firmware doğrulanıyor; birkaç saniye içinde kalıcı hale gelecek. Bu sırada cihazın elektriğini kesmeyin.</p>
        <div class="wifi-connection" id="online-update-status" data-state="waiting" role="status" aria-live="polite">
          <span class="wifi-dot" aria-hidden="true"></span>
          <span class="wifi-copy">
            <strong id="online-update-label">Güncelleme durumu alınıyor</strong>
            <span id="online-update-detail">Lütfen bekleyin…</span>
          </span>
          <code class="wifi-ip" id="online-update-version" hidden></code>
        </div>
        <div class="update-offer" id="online-update-offer" hidden>
          <strong id="online-update-title"></strong>
          <p class="firmware-meta" id="online-update-notes"></p>
        </div>
        <div class="firmware-progress" id="online-update-progress" hidden>
          <div class="firmware-progress-track" role="progressbar" aria-label="Çevrimiçi güncelleme ilerlemesi" aria-valuemin="0" aria-valuemax="100" aria-valuenow="0" id="online-update-track"><span class="firmware-progress-bar" id="online-update-bar"></span></div>
          <output class="firmware-progress-value" id="online-update-value">0%</output>
        </div>
        <label class="sleep-toggle" for="onlineUpdateAuto">
          <input id="onlineUpdateAuto" type="checkbox" value="1">
          Güncellemeleri otomatik denetle (günde bir kez)
        </label>
        <p class="sleep-hint">Cihaz STA ağı üzerinden internete bağlıyken GitHub üzerinde yayınlanan son firmware'i denetler. Yeni sürüm yalnızca siz onayladığınızda indirilip kurulur. Kurulumdan sonra sayfa yeniden açıldığında yeni firmware birkaç saniye içinde kalıcı hale gelir; bundan önce elektrik kesilirse cihaz önceki sürüme döner.</p>
        <div class="actions">
          <button class="button secondary" id="online-update-check" type="button">Şimdi Denetle</button>
          <button class="button" id="online-update-install" type="button" hidden>Güncelle</button>
        </div>
      </div>

      <h3 class="subsection-title">Dosyadan güncelleme</h3>
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

    <footer><span>Yazılım v</span>)HTML"
FIRMWARE_VERSION
R"HTML(</footer>
  </div>
  <div class="status" id="status" role="status" aria-live="polite"></div>
  <script src="/i18n.js"></script>
  <script>
    const I18n = window.StepRgbI18n;
    I18n.init();
    const t = (source, values) => I18n.t(source, values);
    const byId = id => document.getElementById(id);
    const statusBox = byId('status');
    let statusTimer;
    let firmwareUploadActive = false;

    function showStatus(message, error = false) {
      clearTimeout(statusTimer);
      statusBox.textContent = t(message);
      statusBox.className = 'status show' + (error ? ' error' : '');
      statusTimer = setTimeout(() => statusBox.className = 'status', 4500);
    }

    document.querySelectorAll('.tab').forEach(button => button.addEventListener('click', () => {
      document.querySelectorAll('.tab').forEach(item => item.classList.toggle('active', item === button));
      document.querySelectorAll('.panel').forEach(item => item.classList.toggle('active', item.id === button.dataset.target + '-section'));
    }));

    let maximumAlarms = 10;

    function refreshAlarmList() {
      const cards = [...document.querySelectorAll('#alarm-list .alarm-card')];
      cards.forEach((card, index) => {
        card.querySelector('.alarm-title').textContent = t('Alarm {number}', {number:index + 1});
        card.querySelector('.alarm-time').name = `alarmTime${index}`;
        card.querySelector('.alarm-duration').name = `alarmDuration${index}`;
        card.querySelector('.alarm-days').name = `alarmDayMode${index}`;
        card.querySelector('.alarm-day-mask').name = `alarmDayMask${index}`;
        updateAlarmDayControls(card);
      });
      byId('alarm-count').textContent = t('{count} / {maximum} alarm', {count:cards.length,maximum:maximumAlarms});
      byId('alarm-empty').hidden = cards.length !== 0;
      byId('add-alarm').disabled = cards.length >= maximumAlarms;
    }

    function addAlarm(item = {}) {
      const list = byId('alarm-list');
      if (list.children.length >= maximumAlarms) {
        showStatus(t('En fazla {maximum} alarm oluşturabilirsiniz.', {maximum:maximumAlarms}), true);
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
      I18n.localize(card);
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

    let maximumLessons = 20;
    let maximumVacations = 10;
    let holidaysKnownUntil = 2035;
    let savedAlarmMode = 0;
    let schoolStatusData = null;
    const studentBellChoices = [0, 1, 2, 3, 4, 5, 10];
    const pad2 = value => String(value).padStart(2, '0');
    const timeText = minutes => `${pad2(Math.floor(minutes / 60))}:${pad2(minutes % 60)}`;
    function minutesOf(text) {
      const match = /^(\d{1,2}):(\d{2})/.exec(text || '');
      return match ? Number(match[1]) * 60 + Number(match[2]) : null;
    }

    function selectedAlarmMode() {
      const checked = document.querySelector('input[name="alarmMode"]:checked');
      return checked ? Number(checked.value) : 0;
    }

    function updateAlarmMode() {
      const school = selectedAlarmMode() === 1;
      byId('manual-alarm-panel').hidden = school;
      byId('school-bell-panel').hidden = !school;
    }

    function schoolDayMask() {
      return [...document.querySelectorAll('[data-school-day]')].reduce(
        (mask, input) => input.checked ? mask | (1 << Number(input.dataset.schoolDay)) : mask,
        0);
    }

    function updateSchoolDayControls() {
      byId('school-custom-days').hidden = Number(byId('schoolDayMode').value) !== 2;
    }

    function renderStudentBellOptions(value = Number(byId('schoolStudentBell').value) || 0) {
      const select = byId('schoolStudentBell');
      const choices = studentBellChoices.includes(value)
        ? studentBellChoices
        : [...studentBellChoices, value].sort((a, b) => a - b);
      select.replaceChildren(...choices.map(minutes => {
        const option = document.createElement('option');
        option.value = String(minutes);
        option.textContent = minutes ? t('Dersten {minutes} dakika önce', {minutes}) : t('Kapalı');
        return option;
      }));
      select.value = String(value);
    }

    function lessonCards() {
      return [...document.querySelectorAll('.lesson-card')];
    }

    function lessonTimes(card) {
      const hasBreak = !card.querySelector('.lesson-break-field').hidden;
      return {
        start: minutesOf(card.querySelector('.lesson-start').value),
        hasBreak,
        end: hasBreak ? minutesOf(card.querySelector('.lesson-break').value) : null
      };
    }

    // Shows the lesson and break lengths so wrong times stand out at once.
    function refreshLessons() {
      const cards = lessonCards();
      cards.forEach((card, index) => {
        card.querySelector('.alarm-title').textContent = t('{number}. Ders', {number:index + 1});
        const lesson = lessonTimes(card);
        card.querySelector('.lesson-add-break').hidden = lesson.hasBreak;
        card.querySelector('.lesson-remove-break').hidden = !lesson.hasBreak;
        const next = cards[index + 1] ? lessonTimes(cards[index + 1]).start : null;
        const parts = [];
        let error = false;
        if (lesson.start !== null && lesson.end !== null) {
          if (lesson.end > lesson.start) {
            parts.push(t('Ders {minutes} dk', {minutes:lesson.end - lesson.start}));
          } else {
            parts.push(t('Teneffüs zili ders başlangıcından sonra olmalıdır.'));
            error = true;
          }
          if (next === null) parts.push(t('Bu teneffüs zili okul çıkış zili olarak çalar.'));
          else if (next > lesson.end) parts.push(t('Teneffüs {minutes} dk', {minutes:next - lesson.end}));
          else {
            parts.push(t('Sonraki ders teneffüs zilinden sonra başlamalıdır.'));
            error = true;
          }
        } else if (lesson.start !== null && next !== null && next <= lesson.start) {
          parts.push(t('Sonraki ders bu dersten sonra başlamalıdır.'));
          error = true;
        }
        const info = card.querySelector('.lesson-info');
        info.textContent = parts.join(' • ');
        info.classList.toggle('error', error);
      });
      byId('lesson-count').textContent = t('{count} / {maximum} ders', {count:cards.length,maximum:maximumLessons});
      byId('lesson-empty').hidden = cards.length !== 0;
      byId('add-lesson').disabled = cards.length >= maximumLessons;
    }

    function addLesson(item = null) {
      const list = byId('lesson-list');
      if (list.children.length >= maximumLessons) {
        showStatus(t('En fazla {maximum} ders eklenebilir.', {maximum:maximumLessons}), true);
        return;
      }

      const card = document.createElement('div');
      card.className = 'alarm-card lesson-card';
      card.innerHTML = `
        <div class="alarm-card-head">
          <strong class="alarm-title">Ders</strong>
          <button class="alarm-remove" type="button">Sil</button>
        </div>
        <div class="lesson-fields">
          <div class="field"><label>Ders başlangıcı</label><input class="lesson-start" type="time" step="60"></div>
          <div class="field lesson-break-field" hidden><label>Teneffüs zili (ders bitişi)</label><input class="lesson-break" type="time" step="60"></div>
        </div>
        <div class="lesson-actions">
          <button class="link-button lesson-add-break" type="button">+ Teneffüs Ekle</button>
          <button class="link-button lesson-remove-break" type="button" hidden>Teneffüsü Kaldır</button>
        </div>
        <p class="lesson-info"></p>`;

      let start = item && item.start ? item.start : '';
      const breakTime = item && item.break ? item.break : '';
      if (!item) {
        // Suggest the next lesson 10 minutes after the previous break bell.
        const cards = lessonCards();
        const previous = cards.length ? lessonTimes(cards[cards.length - 1]) : null;
        let minutes = 8 * 60 + 30;
        if (previous && previous.end !== null) minutes = previous.end + 10;
        else if (previous && previous.start !== null) minutes = previous.start + 50;
        start = timeText(Math.min(minutes, 23 * 60 + 59));
      }
      card.querySelector('.lesson-start').value = start;
      card.querySelector('.lesson-break').value = breakTime;
      card.querySelector('.lesson-break-field').hidden = !breakTime;
      card.querySelector('.lesson-start').addEventListener('input', refreshLessons);
      card.querySelector('.lesson-break').addEventListener('input', refreshLessons);
      card.querySelector('.lesson-add-break').addEventListener('click', () => {
        // A lesson lasts 40 minutes by default.
        const lessonStart = minutesOf(card.querySelector('.lesson-start').value);
        if (lessonStart !== null) card.querySelector('.lesson-break').value = timeText(Math.min(lessonStart + 40, 23 * 60 + 59));
        card.querySelector('.lesson-break-field').hidden = false;
        refreshLessons();
        card.querySelector('.lesson-break').focus();
      });
      card.querySelector('.lesson-remove-break').addEventListener('click', () => {
        card.querySelector('.lesson-break').value = '';
        card.querySelector('.lesson-break-field').hidden = true;
        refreshLessons();
      });
      card.querySelector('.alarm-remove').addEventListener('click', () => {
        card.remove();
        refreshLessons();
      });
      list.appendChild(card);
      I18n.localize(card);
      refreshLessons();
    }

    function refreshVacations() {
      const cards = [...document.querySelectorAll('.vacation-card')];
      cards.forEach((card, index) => {
        card.querySelector('.alarm-title').textContent = t('Tatil dönemi {number}', {number:index + 1});
      });
      byId('vacation-count').textContent = t('{count} / {maximum} tatil dönemi', {count:cards.length,maximum:maximumVacations});
      byId('vacation-empty').hidden = cards.length !== 0;
      byId('add-vacation').disabled = cards.length >= maximumVacations;
    }

    function addVacation(item = null) {
      const list = byId('vacation-list');
      if (list.children.length >= maximumVacations) {
        showStatus(t('En fazla {maximum} tatil dönemi eklenebilir.', {maximum:maximumVacations}), true);
        return;
      }
      const card = document.createElement('div');
      card.className = 'alarm-card vacation-card';
      card.innerHTML = `
        <div class="alarm-card-head">
          <strong class="alarm-title">Tatil dönemi</strong>
          <button class="alarm-remove" type="button">Sil</button>
        </div>
        <div class="lesson-fields">
          <div class="field"><label>Başlangıç tarihi</label><input class="vacation-start" type="date" min="2000-01-01" max="2099-12-31"></div>
          <div class="field"><label>Bitiş tarihi</label><input class="vacation-end" type="date" min="2000-01-01" max="2099-12-31"></div>
        </div>`;
      card.querySelector('.vacation-start').value = item && item.start ? item.start : '';
      card.querySelector('.vacation-end').value = item && item.end ? item.end : '';
      card.querySelector('.vacation-start').addEventListener('change', event => {
        const end = card.querySelector('.vacation-end');
        if (!end.value || end.value < event.target.value) end.value = event.target.value;
      });
      card.querySelector('.alarm-remove').addEventListener('click', () => {
        card.remove();
        refreshVacations();
      });
      list.appendChild(card);
      I18n.localize(card);
      refreshVacations();
    }

    function renderSchoolBell(data, mode) {
      const school = data || {};
      maximumLessons = Math.max(1, Number(school.maximumLessons) || 20);
      maximumVacations = Math.max(1, Number(school.maximumVacations) || 10);
      document.querySelectorAll('input[name="alarmMode"]').forEach(input => {
        input.checked = Number(input.value) === (Number(mode) === 1 ? 1 : 0);
      });
      byId('schoolDayMode').value = [0, 1, 2].includes(Number(school.dayMode)) ? String(school.dayMode) : '1';
      const mask = Number(school.dayMask) || 31;
      document.querySelectorAll('[data-school-day]').forEach(input => {
        input.checked = (mask & (1 << Number(input.dataset.schoolDay))) !== 0;
      });
      byId('schoolDuration').value = Number(school.duration) || 5;
      renderStudentBellOptions(Number(school.studentBell) || 0);
      byId('schoolSkipHolidays').checked = school.skipHolidays !== false;
      byId('schoolRemembrance').checked = Boolean(school.remembrance);
      byId('lesson-list').replaceChildren();
      (Array.isArray(school.lessons) ? school.lessons : []).slice(0, maximumLessons).forEach(item => addLesson(item));
      byId('vacation-list').replaceChildren();
      (Array.isArray(school.vacations) ? school.vacations : []).slice(0, maximumVacations).forEach(item => addVacation(item));
      updateSchoolDayControls();
      updateAlarmMode();
      refreshLessons();
      refreshVacations();
    }

    function formatDeviceDate(text) {
      const match = /^(\d{4})-(\d{2})-(\d{2})$/.exec(text || '');
      return match ? `${match[3]}.${match[2]}.${match[1]}` : String(text || '');
    }

    function nextDateText(text) {
      const date = new Date(text + 'T00:00:00Z');
      date.setUTCDate(date.getUTCDate() + 1);
      return date.toISOString().slice(0, 10);
    }

    function bellLabel(next) {
      if (next.kind === 'remembrance') return t('10 Kasım saygı duruşu zili');
      if (next.kind === 'break') return t('{number}. ders teneffüs zili', {number:next.lesson});
      if (next.kind === 'studentCall') return t('{number}. ders öğrenci zili', {number:next.lesson});
      return t('{number}. ders zili', {number:next.lesson});
    }

    // Groups consecutive days of the same holiday into one line.
    function renderHolidayList(status) {
      const list = byId('holiday-list');
      const days = status && status.clock && Array.isArray(status.holidays) ? status.holidays : [];
      const groups = [];
      days.forEach(day => {
        const last = groups[groups.length - 1];
        if (last && last.name === day.name && nextDateText(last.end) === day.date) last.end = day.date;
        else groups.push({name:day.name, start:day.date, end:day.date, half:Boolean(day.half)});
      });
      const title = document.createElement('li');
      title.className = 'holiday-title';
      title.textContent = t('Yaklaşan resmi tatiller');
      list.replaceChildren(title, ...groups.slice(0, 6).map(group => {
        const item = document.createElement('li');
        const time = document.createElement('time');
        time.textContent = group.start === group.end
          ? formatDeviceDate(group.start)
          : `${formatDeviceDate(group.start)} – ${formatDeviceDate(group.end)}`;
        const name = document.createElement('span');
        name.textContent = t(group.name) + (group.half ? ' (' + t('13:00 sonrası') + ')' : '');
        item.append(time, name);
        return item;
      }));
      list.hidden = groups.length === 0;
      byId('holiday-range').textContent = t('Dini bayram tarihleri Diyanet takvimine göre {year} yılına kadar tanımlıdır.', {year:holidaysKnownUntil});
    }

    function renderSchoolStatus() {
      const status = schoolStatusData;
      renderHolidayList(status);
      if (!status) return;
      let state = 'waiting';
      let label;
      let detail = '';
      if (savedAlarmMode !== 1) {
        label = t('Okul saati etkin değil');
        detail = t('Okul saati seçilip kaydedildiğinde zil programı çalışmaya başlar.');
      } else if (!status.clock) {
        state = 'failed';
        label = t('Saat okunamıyor');
        detail = t('DS3231 saati bulunamadığı için okul zili çalmaz.');
      } else {
        const holiday = status.holiday ? t(status.holiday) : '';
        state = status.state === 'schoolDay' ? 'connected' : 'paused';
        if (status.state === 'schoolDay') label = t('Bugün okul günü');
        else if (status.state === 'halfDayHoliday') {
          label = t('Bugün yarım gün resmi tatil');
          detail = t('{holiday}: saat 13:00 sonrasında zil çalmaz.', {holiday}) + ' ';
        } else if (status.state === 'publicHoliday') {
          label = t('Bugün resmi tatil');
          detail = holiday + '. ';
        } else if (status.state === 'vacation') label = t('Bugün tatil dönemi');
        else label = t('Bugün okul günü değil');
        if (status.next) {
          const day = status.next.date === status.today
            ? t('bugün')
            : status.next.date === nextDateText(status.today) ? t('yarın') : formatDeviceDate(status.next.date);
          detail += t('Sıradaki zil: {day} {time} — {bell}', {day, time:status.next.time, bell:bellLabel(status.next)});
        } else {
          detail += t('Programda çalacak zil yok.');
        }
      }
      byId('school-status').dataset.state = state;
      byId('school-status-label').textContent = label;
      byId('school-status-detail').textContent = detail;
    }

    // Checks the school bell form and adds it to the request body.
    function appendSchoolBellFields(body) {
      const cards = lessonCards();
      const lead = Number(byId('schoolStudentBell').value) || 0;
      if (cards.length === 0 && !byId('schoolRemembrance').checked) {
        throw new Error(t('Okul saati için en az bir ders ekleyin.'));
      }
      let previousEnd = null;
      cards.forEach((card, index) => {
        const number = index + 1;
        const lesson = lessonTimes(card);
        if (lesson.start === null || (lesson.hasBreak && lesson.end === null)) {
          throw new Error(t('{number}. dersin saati eksik.', {number}));
        }
        if (lesson.hasBreak && lesson.end <= lesson.start) {
          throw new Error(t('{number}. dersin teneffüs zili ders başlangıcından sonra olmalıdır.', {number}));
        }
        if (previousEnd !== null && lesson.start <= previousEnd) {
          throw new Error(t('{number}. ders, bir önceki dersin teneffüs zilinden sonra başlamalıdır.', {number}));
        }
        if (lead > 0 && (lesson.start < lead || (previousEnd !== null && lesson.start - lead <= previousEnd))) {
          throw new Error(t('{number}. dersin öğrenci zili bir önceki dersin teneffüs zilinden sonra çalmalıdır; öğrenci zili süresini kısaltın.', {number}));
        }
        previousEnd = lesson.hasBreak ? lesson.end : lesson.start;
        body.set(`lessonStart${index}`, timeText(lesson.start));
        body.set(`lessonBreak${index}`, lesson.hasBreak ? timeText(lesson.end) : '');
      });
      const duration = Number(byId('schoolDuration').value);
      if (!Number.isInteger(duration) || duration < 1 || duration > 3600) {
        throw new Error(t('Zil süresi 1-3600 saniye olmalıdır.'));
      }
      if (Number(byId('schoolDayMode').value) === 2 && schoolDayMask() === 0) {
        throw new Error(t('Okul saati için en az bir gün seçilmelidir.'));
      }
      const vacations = [...document.querySelectorAll('.vacation-card')];
      vacations.forEach((card, index) => {
        const number = index + 1;
        const start = card.querySelector('.vacation-start').value;
        const end = card.querySelector('.vacation-end').value;
        if (!start || !end) throw new Error(t('Tatil dönemi {number} için başlangıç ve bitiş tarihi girin.', {number}));
        if (end < start) throw new Error(t('Tatil dönemi {number}: bitiş tarihi başlangıçtan önce olamaz.', {number}));
        body.set(`vacationStart${index}`, start);
        body.set(`vacationEnd${index}`, end);
      });
      body.set('schoolLessonCount', String(cards.length));
      body.set('schoolDayMode', byId('schoolDayMode').value);
      body.set('schoolDayMask', String(schoolDayMask()));
      body.set('schoolDuration', String(duration));
      body.set('schoolStudentBell', String(lead));
      body.set('schoolSkipHolidays', byId('schoolSkipHolidays').checked ? '1' : '0');
      body.set('schoolRemembrance', byId('schoolRemembrance').checked ? '1' : '0');
      body.set('vacationCount', String(vacations.length));
    }

    document.querySelectorAll('input[name="alarmMode"]').forEach(input => input.addEventListener('change', updateAlarmMode));
    byId('schoolDayMode').addEventListener('change', updateSchoolDayControls);
    document.querySelectorAll('[data-school-day]').forEach(input => input.addEventListener('change', () => {
      if (schoolDayMask() === 0) {
        input.checked = true;
        showStatus('Okul saati için en az bir gün seçilmelidir.', true);
      }
    }));
    byId('add-lesson').addEventListener('click', () => addLesson());
    byId('add-vacation').addEventListener('click', () => addVacation());
    byId('test-bell').addEventListener('click', async () => {
      const seconds = selectedAlarmMode() === 1
        ? Math.max(1, Math.min(30, Number(byId('schoolDuration').value) || 5))
        : 3;
      try {
        const result = await request('/api/alarm/test', {
          method: 'POST',
          headers: {'Content-Type':'application/x-www-form-urlencoded'},
          body: new URLSearchParams({seconds:String(seconds)})
        });
        showStatus(I18n.server(result.message, 'İşlem tamamlandı.'));
      } catch (error) { showStatus(error.message, true); }
    });

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
      const data = await response.json().catch(() => ({ok:false,message:'Geçersiz cihaz yanıtı.'}));
      if (!response.ok || ('ok' in data && !data.ok)) throw new Error(I18n.server(data.message, 'İşlem başarısız.'));
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
        ? ' ' + t('Sonraki denemeye {seconds} saniye kaldı.', {seconds:wifiRetrySeconds})
        : ' ' + t('Yeni bağlantı denemesi başlatılıyor…');
    }

    function updateWifiStatusDetail() {
      if (!wifiStatusData) return;
      const data = wifiStatusData;
      const state = wifiStateLabels[data.state] ? data.state : 'waiting';
      let detail = t('Bir sonraki otomatik bağlantı denemesi bekleniyor.') + wifiRetryText();
      if (state === 'disabled') detail = t('Bağlanılacak bir STA ağı kaydedilmemiş.');
      if (state === 'paused') detail = t('Yerel AP arayüzüne bir cihaz bağlı olduğu için yeni STA denemeleri bekletiliyor.');
      if (state === 'connecting') detail = t('“{ssid}” ağına bağlanılıyor…', {ssid:data.ssid});
      if (state === 'connected') detail = t('“{ssid}” ağına bağlandı.', {ssid:data.ssid});
      if (state === 'failed') {
        detail = t(wifiFailureLabels[data.reason] || 'Bağlantı kurulamadı.');
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
      byId('wifi-state-label').textContent = t(wifiStateLabels[state]);
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
        byId('wifi-signal-text').textContent = t('Sinyal {quality}% ({rssi} dBm)', {quality,rssi});
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

    // The date/time fields show a running clock: device time plus the time
    // elapsed since it was read. A manually entered value keeps running from
    // the moment it was entered, so saving later never writes a stale time.
    const clockState = {offsetMs: null, fromDevice: true, editing: false};

    function localTimestamp(date, time) {
      const dateMatch = /^(\d{4})-(\d{2})-(\d{2})$/.exec(date || '');
      const timeMatch = /^(\d{2}):(\d{2})(?::(\d{2}))?$/.exec(time || '');
      if (!dateMatch || !timeMatch) return null;
      return new Date(Number(dateMatch[1]), Number(dateMatch[2]) - 1, Number(dateMatch[3]),
        Number(timeMatch[1]), Number(timeMatch[2]), Number(timeMatch[3] || 0)).getTime();
    }

    function renderClock() {
      if (clockState.offsetMs === null || clockState.editing) return;
      const now = new Date(Date.now() + clockState.offsetMs);
      const pad = value => String(value).padStart(2, '0');
      byId('date').value = `${now.getFullYear()}-${pad(now.getMonth() + 1)}-${pad(now.getDate())}`;
      byId('time').value = `${pad(now.getHours())}:${pad(now.getMinutes())}:${pad(now.getSeconds())}`;
    }

    function captureManualClock() {
      const timestamp = localTimestamp(byId('date').value, byId('time').value);
      if (timestamp === null) return;
      clockState.offsetMs = timestamp - Date.now();
      clockState.fromDevice = false;
    }

    ['date', 'time'].forEach(id => {
      const input = byId(id);
      input.addEventListener('focus', () => { clockState.editing = true; });
      input.addEventListener('change', captureManualClock);
      input.addEventListener('blur', () => { captureManualClock(); clockState.editing = false; renderClock(); });
    });

    const timeZonePresets = [...byId('timeZonePreset').options].map(option => option.value).filter(value => value !== 'custom');

    function selectedTimeZone() {
      const preset = byId('timeZonePreset').value;
      return preset === 'custom' ? byId('timeZone').value.trim() : preset;
    }

    function updateTimeZoneControls() {
      byId('custom-time-zone').hidden = byId('timeZonePreset').value !== 'custom';
    }
    byId('timeZonePreset').addEventListener('change', updateTimeZoneControls);

    let timeSyncData = null;
    function renderTimeSyncStatus() {
      const data = timeSyncData;
      if (!data) return;
      let text = t('NTP eşitlemesi kapalı.');
      if (data.enabled && data.synchronized) {
        const minutes = Math.floor((Number(data.secondsSinceSync) || 0) / 60);
        text = minutes < 1
          ? t('Saat az önce NTP ile eşitlendi.')
          : t('Son NTP eşitlemesi {minutes} dakika önce yapıldı.', {minutes});
      } else if (data.enabled && data.stationConnected) {
        text = t('NTP sunucusundan yanıt bekleniyor…');
      } else if (data.enabled) {
        text = t('NTP için cihazın STA ağına bağlı olması gerekir.');
      }
      byId('ntp-status').textContent = text;
    }

    function updateSensorOptions() {
      const type = byId('sensorType').value;
      document.querySelectorAll('[data-sensor-options]').forEach(fieldset => {
        fieldset.hidden = fieldset.dataset.sensorOptions !== type;
      });
      byId('humidity-offset-field').hidden = type === '1';
    }
    byId('sensorType').addEventListener('change', updateSensorOptions);

    function renderSensorStatus(data) {
      const box = byId('sensor-status');
      const sensor = data && data.sensor ? data.sensor : {name: '—', address: 0, humidity: false, missingText: '?'};
      const address = '0x' + Number(sensor.address).toString(16).toUpperCase().padStart(2, '0');
      const reading = byId('sensor-reading');
      if (!data || !data.found) {
        box.dataset.state = 'failed';
        byId('sensor-status-label').textContent = t('{sensor} bulunamadı', {sensor: sensor.name});
        byId('sensor-status-detail').textContent = t('{address} adresinde cevap yok; panelde {text} gösteriliyor. Bağlantıyı ve adres seçimini kontrol edin.', {address, text: sensor.missingText});
        reading.hidden = true;
        return;
      }
      box.dataset.state = data.ready ? 'connected' : 'connecting';
      byId('sensor-status-label').textContent = t('{sensor} bulundu', {sensor: sensor.name});
      byId('sensor-status-detail').textContent = data.ready
        ? t('{address} adresinden ölçüm alınıyor.', {address})
        : t('İlk ölçüm bekleniyor…');
      const parts = [];
      if (Number.isFinite(data.temperature)) parts.push(Number(data.temperature).toFixed(1) + ' °C');
      if (sensor.humidity && Number.isFinite(data.humidity)) parts.push(Number(data.humidity).toFixed(1) + ' %RH');
      reading.textContent = parts.join(' • ');
      reading.hidden = parts.length === 0;
    }

    let sensorStatusData = null;
    async function loadSensorStatus() {
      if (firmwareUploadActive) return;
      try { sensorStatusData = await request('/api/temperature'); }
      catch (error) { sensorStatusData = null; }
      renderSensorStatus(sensorStatusData);
    }

    let wifiSettings = {staSsid: '', staPasswordSet: false};
    function renderStaPasswordPlaceholder() {
      byId('staPassword').placeholder = t(wifiSettings.staPasswordSet
        ? 'Kayıtlı parola korunur; değiştirmek için yazın'
        : 'Açık ağ için boş bırakılabilir');
    }

    function applySettings(data, initial) {
      if (initial) {
        byId('scan').value = data.panel.scan;
        byId('rows').value = data.panel.rows;
        byId('columns').value = data.panel.columns;
        byId('brightness').value = data.panel.brightness;
        byId('brightness-value').value = data.panel.brightness;
        selectConnection(data.panel.connectionStyle);
        updatePanelCount();
        byId('sleepEnabled').checked = Boolean(data.sleep.enabled);
        byId('sleepStart').value = data.sleep.start;
        byId('sleepEnd').value = data.sleep.end;
        byId('sleepDayMode').value = data.sleep.dayMode;
        byId('ntpEnabled').checked = Boolean(data.timeSync.enabled);
        byId('ntpServer').value = data.timeSync.server;
        const knownZone = timeZonePresets.includes(data.timeSync.timeZone);
        byId('timeZonePreset').value = knownZone ? data.timeSync.timeZone : 'custom';
        byId('timeZone').value = knownZone ? '' : data.timeSync.timeZone;
        updateTimeZoneControls();
        const sensor = data.temperatureSensor;
        byId('sensorType').value = sensor.type;
        byId('sensorInterval').value = sensor.interval;
        byId('temperatureOffset').value = (Number(sensor.temperatureOffset) / 10).toFixed(1);
        byId('humidityOffset').value = (Number(sensor.humidityOffset) / 10).toFixed(1);
        byId('sht40Address').value = sensor.sht40Address;
        byId('sht40Precision').value = sensor.sht40Precision;
        byId('tmp112Address').value = sensor.tmp112Address;
        byId('tmp112ConversionRate').value = sensor.tmp112ConversionRate;
        byId('tmp112ExtendedMode').checked = Boolean(sensor.tmp112ExtendedMode);
        byId('hdc2022Address').value = sensor.hdc2022Address;
        byId('hdc2022TemperatureResolution').value = sensor.hdc2022TemperatureResolution;
        byId('hdc2022HumidityResolution').value = sensor.hdc2022HumidityResolution;
        updateSensorOptions();
        renderAlarms(data.alarms);
        renderSchoolBell(data.schoolBell, data.alarmMode);
        byId('slaveAddress').value = data.communication.slaveAddress;
        byId('baudRate').value = data.communication.baudRate;
        byId('dataBits').value = data.communication.dataBits;
        byId('parity').value = data.communication.parity;
        byId('handshake').value = data.communication.handshake;
        byId('apSsid').value = data.wifi.apSsid;
        byId('apPassword').value = '';
        byId('staSsid').value = data.wifi.staSsid;
        byId('staPassword').value = '';
        byId('staAuthentication').checked = Boolean(data.wifi.staAuthentication);
        wifiSettings = {staSsid: data.wifi.staSsid, staPasswordSet: Boolean(data.wifi.staPasswordSet)};
        renderStaPasswordPlaceholder();
      }
      byId('default-password-warning').hidden = !data.wifi.apPasswordDefault;

      const rtcAvailable = Boolean(data.clock.available);
      byId('rtc-missing').hidden = rtcAvailable;
      ['date', 'time', 'browser-time', 'clock-submit'].forEach(id => { byId(id).disabled = !rtcAvailable; });
      const deviceTimestamp = rtcAvailable ? localTimestamp(data.clock.date, data.clock.time) : null;
      if (deviceTimestamp !== null && clockState.fromDevice && !clockState.editing) {
        clockState.offsetMs = deviceTimestamp - Date.now();
        renderClock();
      }

      timeSyncData = data.timeSync;
      renderTimeSyncStatus();

      savedAlarmMode = Number(data.alarmMode) === 1 ? 1 : 0;
      if (data.schoolBell) {
        holidaysKnownUntil = Number(data.schoolBell.holidaysKnownUntil) || holidaysKnownUntil;
        schoolStatusData = data.schoolBell.status || null;
      }
      renderSchoolStatus();
    }

    async function loadSettings(initial = true) {
      try {
        applySettings(await request('/api/settings'), initial);
      } catch (error) {
        if (initial) showStatus(error.message, true);
      }
    }

    async function submitForm(event, endpoint) {
      event.preventDefault();
      try {
        if (endpoint === '/api/settings/alarms') {
          refreshAlarmList();
          const invalidCustomDays = [...document.querySelectorAll('#alarm-list .alarm-card')].some(card =>
            Number(card.querySelector('.alarm-days').value) === 2
            && alarmDayMask(card) === 0);
          if (invalidCustomDays) {
            throw new Error(t('Özel alarm için en az bir gün seçilmelidir.'));
          }
        }
        if (endpoint === '/api/settings/clock') {
          if (clockState.editing) captureManualClock();
          clockState.editing = false;
          renderClock();
        }
        const body = new URLSearchParams(new FormData(event.currentTarget));
        if (endpoint === '/api/settings/temperature-sensor') {
          body.set('temperatureOffset', String(Math.round(Number(byId('temperatureOffset').value) * 10)));
          body.set('humidityOffset', String(Math.round(Number(byId('humidityOffset').value || 0) * 10)));
          body.set('tmp112ExtendedMode', byId('tmp112ExtendedMode').checked ? '1' : '0');
        }
        if (endpoint === '/api/settings/temperature-sensor') {
          const temperatureOffset = Math.round(Number(byId('temperatureOffset').value) * 10);
          const humidityOffset = Math.round(Number(byId('humidityOffset').value || 0) * 10);
          if (!Number.isFinite(temperatureOffset) || Math.abs(temperatureOffset) > 100) throw new Error(t('Sıcaklık düzeltmesi -10.0 ile 10.0 °C arasında olmalıdır.'));
          if (!Number.isFinite(humidityOffset) || Math.abs(humidityOffset) > 200) throw new Error(t('Nem düzeltmesi -20.0 ile 20.0 %RH arasında olmalıdır.'));
        }
        if (endpoint === '/api/settings/sleep') {
          body.set('sleepEnabled', byId('sleepEnabled').checked ? '1' : '0');
        }
        if (endpoint === '/api/settings/time-sync') {
          body.set('ntpEnabled', byId('ntpEnabled').checked ? '1' : '0');
          body.set('timeZone', selectedTimeZone());
        }
        if (endpoint === '/api/settings/wifi') {
          // Stored passwords are never sent to the browser; an empty field
          // keeps the stored one. An empty STA password for a new SSID means
          // an open network.
          body.set('apPasswordKeep', byId('apPassword').value === '' ? '1' : '0');
          const keepStaPassword = byId('staPassword').value === ''
            && byId('staSsid').value === wifiSettings.staSsid
            && wifiSettings.staPasswordSet;
          body.set('staPasswordKeep', keepStaPassword ? '1' : '0');
          body.set('staAuthentication', byId('staAuthentication').checked ? '1' : '0');
        }
        if (endpoint === '/api/settings/alarms') {
          const alarmCards = [...document.querySelectorAll('#alarm-list .alarm-card')];
          body.set('alarmCount', alarmCards.length);
          alarmCards.forEach((card, index) => {
            body.set(`alarmDayMask${index}`, String(alarmDayMask(card)));
          });
          // The stored school schedule is kept when manual alarms are saved.
          if (selectedAlarmMode() === 1) appendSchoolBellFields(body);
        }
        const result = await request(endpoint, {method: 'POST', headers: {'Content-Type':'application/x-www-form-urlencoded'}, body});
        showStatus(I18n.server(result.message, 'İşlem tamamlandı.'));
        if (endpoint === '/api/settings/clock') clockState.fromDevice = true;
        if (endpoint === '/api/settings/time-sync') setTimeout(() => loadSettings(false), 3000);
        if (endpoint === '/api/settings/temperature-sensor') setTimeout(loadSensorStatus, 300);
        if (endpoint === '/api/settings/alarms') loadSettings(false);
      } catch (error) { showStatus(error.message, true); }
    }

    byId('panel-form').addEventListener('submit', event => submitForm(event, '/api/settings/panel'));
    byId('clock-form').addEventListener('submit', event => submitForm(event, '/api/settings/clock'));
    byId('time-sync-form').addEventListener('submit', event => submitForm(event, '/api/settings/time-sync'));
    byId('sleep-form').addEventListener('submit', event => submitForm(event, '/api/settings/sleep'));
    byId('sensor-form').addEventListener('submit', event => submitForm(event, '/api/settings/temperature-sensor'));
    byId('alarm-form').addEventListener('submit', event => submitForm(event, '/api/settings/alarms'));
    byId('communication-form').addEventListener('submit', event => submitForm(event, '/api/settings/communication'));
    byId('wifi-form').addEventListener('submit', event => submitForm(event, '/api/settings/wifi'));
    byId('add-alarm').addEventListener('click', () => addAlarm());

    const firmwareForm = byId('firmware-form');
    const firmwareFile = byId('firmwareFile');
    const firmwareSubmit = byId('firmware-submit');

    function firmwareFileSize(bytes) {
      if (bytes < 1024) return `${bytes} ${t('bayt')}`;
      if (bytes < 1024 * 1024) return `${(bytes / 1024).toFixed(1)} KB`;
      return `${(bytes / (1024 * 1024)).toFixed(2)} MB`;
    }

    function setFirmwareProgress(percent, message) {
      const value = Math.max(0, Math.min(100, Math.round(percent)));
      byId('firmware-progress').hidden = false;
      byId('firmware-progress-bar').style.width = `${value}%`;
      byId('firmware-progress-value').value = `${value}%`;
      byId('firmware-progress-track').setAttribute('aria-valuenow', String(value));
      byId('firmware-progress-text').textContent = t(message);
    }

    function validateFirmwareFile(showError = false) {
      const file = firmwareFile.files[0];
      const valid = Boolean(file && file.name === 'firmware.bin');
      firmwareSubmit.disabled = firmwareUploadActive || !valid;

      if (!file) {
        byId('firmware-file-meta').innerHTML = '<strong>' + t('Gerekli dosya adı:') + '</strong> firmware.bin';
      } else if (valid) {
        byId('firmware-file-meta').textContent = `${file.name} • ${firmwareFileSize(file.size)}`;
      } else {
        byId('firmware-file-meta').innerHTML = '<strong>' + t('Geçersiz dosya.') + '</strong> ' + t('Dosya adı tam olarak firmware.bin olmalıdır.');
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
          setFirmwareProgress(100, I18n.server(result.message, 'Güncelleme tamamlandı. Cihaz yeniden başlatılıyor.'));
          showStatus(I18n.server(result.message, 'Güncelleme tamamlandı.'));
          return;
        }

        firmwareUploadActive = false;
        firmwareFile.disabled = false;
        validateFirmwareFile();
        const message = I18n.server(result.message, 'Firmware güncellenemedi.');
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

    let onlineUpdateData = null;
    let onlineUpdateTimer = 0;
    let installingVersion = '';

    function onlineUpdateBusy(data) {
      return Boolean(data) && ['checking', 'downloading', 'installed'].includes(data.state);
    }

    function formatCheckAge(seconds) {
      if (seconds < 60) return t('az önce');
      if (seconds < 3600) return t('{minutes} dakika önce', {minutes:Math.floor(seconds / 60)});
      return t('{hours} saat önce', {hours:Math.floor(seconds / 3600)});
    }

    function renderOnlineUpdate() {
      const data = onlineUpdateData;
      if (!data) return;
      let state = 'waiting';
      let label;
      let detail;
      if (data.state === 'checking') {
        state = 'connecting';
        label = t('Güncellemeler denetleniyor');
        detail = t('GitHub sunucusuna bağlanılıyor…');
      } else if (data.state === 'downloading') {
        state = 'connecting';
        label = t('Güncelleme indiriliyor');
        detail = t('{version} sürümü indirilip kuruluyor. Bu sırada cihazın elektriğini kesmeyin.', {version:data.latest});
      } else if (data.state === 'installed') {
        state = 'connected';
        label = t('Güncelleme kuruldu');
        detail = t('Cihaz yeniden başlatılıyor; sayfa cihaz açılınca kendiliğinden yenilenir.');
      } else if (data.state === 'failed') {
        state = 'failed';
        label = t('Güncelleme işlemi başarısız');
        detail = I18n.server(data.error, 'İşlem başarısız.');
      } else if (data.updateAvailable) {
        state = 'paused';
        label = t('Yeni sürüm bulundu');
        detail = t('{version} sürümü kurulabilir. Yüklü sürüm: {current}.', {version:data.latest, current:data.current});
      } else if (data.state === 'upToDate') {
        state = 'connected';
        label = t('Yazılım güncel');
        detail = t('Yüklü sürüm GitHub üzerindeki en son sürüm.');
      } else {
        label = t('Henüz denetlenmedi');
        detail = data.stationConnected
          ? t('Otomatik denetim, STA bağlantısından birkaç dakika sonra yapılır.')
          : t('Güncellemeleri denetlemek için cihazın STA ağı üzerinden internete bağlı olması gerekir.');
      }
      if (!onlineUpdateBusy(data) && data.secondsSinceCheck >= 0) {
        detail += ' ' + t('Son denetim: {age}.', {age:formatCheckAge(Number(data.secondsSinceCheck))});
      }
      byId('online-update-status').dataset.state = state;
      byId('online-update-label').textContent = label;
      byId('online-update-detail').textContent = detail;
      byId('online-update-version').hidden = false;
      byId('online-update-version').textContent = 'v' + data.current;

      const rollback = byId('online-update-rollback');
      rollback.hidden = !data.rolledBack;
      rollback.textContent = data.rolledBack
        ? t('{version} sürümü başlatılamadı; cihaz önceki sürüme döndü.', {version:data.rolledBack})
        : '';

      const offer = Boolean(data.updateAvailable) && data.state !== 'installed';
      byId('online-update-offer').hidden = !offer;
      if (offer) {
        const size = Number(data.size) ? ' • ' + firmwareFileSize(Number(data.size)) : '';
        byId('online-update-title').textContent = t('Yeni sürüm: {version}', {version:data.latest}) + size;
        byId('online-update-notes').textContent = data.notes ? t('Son değişiklik: {notes}', {notes:data.notes}) : '';
        byId('online-update-notes').hidden = !data.notes;
      }
      const install = byId('online-update-install');
      install.hidden = !offer;
      install.disabled = onlineUpdateBusy(data);
      install.textContent = t('{version} Sürümüne Güncelle', {version:data.latest});
      byId('online-update-check').disabled = onlineUpdateBusy(data);
      byId('onlineUpdateAuto').checked = Boolean(data.automatic);

      const showProgress = data.state === 'downloading' || data.state === 'installed';
      byId('online-update-progress').hidden = !showProgress;
      const percent = Math.max(0, Math.min(100, Number(data.progress) || 0));
      byId('online-update-bar').style.width = percent + '%';
      byId('online-update-value').value = percent + '%';
      byId('online-update-track').setAttribute('aria-valuenow', String(percent));
      byId('firmware-tab-badge').hidden = !data.updateAvailable || data.state === 'installed';
      // Older firmware does not report the field; treat it as confirmed.
      byId('online-update-pending').hidden = data.confirmed !== false;
    }

    function applyOnlineUpdate(data) {
      if (installingVersion && data.current === installingVersion) {
        // The device came back with the new firmware; load its portal.
        installingVersion = '';
        location.reload();
        return;
      }
      if (data.state === 'installed' || data.state === 'downloading') installingVersion = data.latest;
      const wasPending = Boolean(onlineUpdateData) && onlineUpdateData.confirmed === false;
      onlineUpdateData = data;
      renderOnlineUpdate();
      if (wasPending && data.confirmed !== false) showStatus('Yeni firmware kalıcı hale getirildi.');
    }

    async function loadOnlineUpdate() {
      clearTimeout(onlineUpdateTimer);
      if (!firmwareUploadActive) {
        try { applyOnlineUpdate(await request('/api/firmware/online')); }
        catch (error) { /* Retried by the next poll, e.g. while the device restarts. */ }
      }
      const fast = onlineUpdateBusy(onlineUpdateData)
        || Boolean(installingVersion)
        || (Boolean(onlineUpdateData) && onlineUpdateData.confirmed === false);
      onlineUpdateTimer = setTimeout(loadOnlineUpdate, fast ? 2000 : 15000);
    }

    async function sendOnlineUpdate(fields) {
      try {
        const data = await request('/api/firmware/online', {
          method: 'POST',
          headers: {'Content-Type':'application/x-www-form-urlencoded'},
          body: new URLSearchParams(fields)
        });
        applyOnlineUpdate(data);
        if (data.message) showStatus(I18n.server(data.message, 'İşlem tamamlandı.'));
      } catch (error) { showStatus(error.message, true); }
      loadOnlineUpdate();
    }

    byId('online-update-check').addEventListener('click', () => sendOnlineUpdate({action:'check'}));
    byId('online-update-install').addEventListener('click', () => {
      if (!onlineUpdateData || !onlineUpdateData.updateAvailable) return;
      if (!confirm(t('Cihaz {version} sürümüne güncellenecek ve yeniden başlayacak. Güncelleme sırasında cihazın elektriğini kesmeyin. Devam edilsin mi?', {version:onlineUpdateData.latest}))) return;
      sendOnlineUpdate({action:'install'});
    });
    byId('onlineUpdateAuto').addEventListener('change', event => sendOnlineUpdate({action:'auto', enabled:event.target.checked ? '1' : '0'}));

    document.querySelectorAll('[data-color]').forEach(button => button.addEventListener('click', async () => {
      try {
        const result = await request('/api/panel/test?color=' + button.dataset.color, {method: 'POST'});
        showStatus(I18n.server(result.message, 'İşlem tamamlandı.'));
      } catch (error) { showStatus(error.message, true); }
    }));

    byId('browser-time').addEventListener('click', () => {
      clockState.offsetMs = 0;
      clockState.fromDevice = false;
      clockState.editing = false;
      renderClock();
    });

    refreshAlarmList();
    renderStudentBellOptions(0);
    refreshLessons();
    refreshVacations();
    loadSettings();
    loadWifiStatus();
    loadOnlineUpdate();
    I18n.onChange(() => {
      renderOnlineUpdate();
      refreshAlarmList();
      refreshLessons();
      refreshVacations();
      renderStudentBellOptions();
      renderSchoolStatus();
      if (wifiStatusData) renderWifiStatus(wifiStatusData);
      renderTimeSyncStatus();
      renderStaPasswordPlaceholder();
      renderSensorStatus(sensorStatusData);
      validateFirmwareFile();
    });
    setInterval(loadWifiStatus, 2000);
    loadSensorStatus();
    setInterval(loadSensorStatus, 2000);
    setInterval(updateWifiCountdown, 1000);
    setInterval(renderClock, 1000);
    // Keeps the device clock and NTP status current without touching fields
    // the user may be editing.
    setInterval(() => { if (!firmwareUploadActive) loadSettings(false); }, 30000);
  </script>
</body>
</html>)HTML";

} // namespace PortalPage
