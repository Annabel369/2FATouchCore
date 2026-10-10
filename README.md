# 2FATouchCore

**Core library for the 2FATouch (Creeper Auth) device** running on ESP32 CYD (Cheap Yellow Display — ESP32-2432S028R).

Includes everything you need to build a complete standalone touch-based 2FA / TOTP authenticator with TFT, weather display, SD card file manager, captive web UI, Creeper animations, and QR-code generation.

---

## 🔥 Key Features

| Feature | Description |
|---|---|
| **Multi-Language (i18n)** | Built-in **English (US)** and **Portuguese (BR)** support for *all* UI layers: TFT screens, HTTP responses, and static HTML pages served from SD card. Toggle via a single global variable. |
| **TOTP / 2FA** | RFC6238-compatible time-based one-time passwords with touch keyboard and vault. |
| **TFT + Touch** | Complete 240×320 ILI9341 + XPT2046 UI stack: Wi-Fi scan, on-screen keyboard, loading screens, weather widget, Creeper animations. |
| **Captive Web UI** | Fully functional web interface with SD streaming, SD card file browser (list/view/edit), upload, 2FA management, YubiKey emulation, PC stats and vault mode. |
| **QR Code Engine** | Fast, small-footprint QR generator (no external deps). |
| **Weather / NTP** | Integrated OpenWeather/Weather map + NTPClient display with wind/beaufort classification. |
| **SD Card I/O** | Full file browser, editor (`.txt`/`.html`/`.json`), upload & delete over HTTP. |
| **Low RAM design** | i18n and SD pages use zero server-side placeholder replacement — streaming works with tiny heap footprint. |

---

## 📦 Components Included

| File | What it does |
|---|---|
| **[`2FATouchCore.h`](src/2FATouchCore.h)** | Main library header — includes everything (pulls `lang.h`, `qrcode.h`, `allfuc.h`). |
| **[`lang.h`](src/lang.h) + [`lang.cpp`](src/lang.cpp)** | **NEW in v1.1** — Multi-language subsystem. Defines `CoreLanguage` enum, global `GlobalLanguage` variable, and the `tr(pt_BR, en_US)` inline helper. |
| **[`allfuc.h`](src/allfuc.h)** | 80+ complete system functions: TFT screens, weather logic, TOTP helpers, 2FA vault, touch handlers, ALL WebServer route handlers (`handleLoginRoute`, `handleListHTML`, `handleEditFile`, `handleUpload`, `handleListJSON`, **`handleI18nJS`**, …). |
| **`qrcode.h` / `qrcode.c`** | High-performance QR code generator (no external dependencies). |

---

## 🚀 Quick Start

### 1. Install dependencies (Arduino IDE / PlatformIO)

Required libraries (see [`library.properties`](library.properties)):
- `TFT_eSPI` (Bodmer) — configure your `User_Setup.h` for the CYD (ILI9341, XPT2046, SPI pins)
- `ESP32FtpServer`
- `ArduinoJson`
- `NTPClient`
- `ESP32Servo`
- `XPT2046_Touchscreen`
- `TJpg_Decoder`

### 2. Minimal working sketch

Copy the template below or open the bundled example:
**`File → Examples → 2FATouchCore → 2FATouchCoreExample`**

```cpp
#include <WiFi.h>
#include <WebServer.h>
#include <TFT_eSPI.h>
#include <2FATouchCore.h>

// =============================================================
// (OPTIONAL) Override default pins here, OR put them in allconfigs.h.
// If you skip this section, safe CYD defaults are used:
//     TFT_BL=4, PIN_RED=16, PIN_GREEN=17, PIN_BLUE=5
// =============================================================
// #define TFT_BL    4
// #define PIN_RED  16
// #define PIN_GREEN 17
// #define PIN_BLUE  5

// =============================================================
// Global objects the library expects you to declare
// (do this BEFORE including 2FATouchCore.h or at sketch top)
// =============================================================
TFT_eSPI  tft    = TFT_eSPI();
WebServer server = WebServer(80);

bool forceRedraw  = false;
int  displayMode  = 0;
int  wifiScanPage = 0;

void setup() {
  Serial.begin(115200);
  delay(200);

  // ---------------------------------------------------------
  // PICK A LANGUAGE (NEW in v1.1 — Multi-Language!)
  //   LANG_PT_BR  = Brazilian Portuguese (default)
  //   LANG_EN_US  = American English
  // ---------------------------------------------------------
  // GlobalLanguage = LANG_EN_US;  // uncomment for English
    GlobalLanguage = LANG_PT_BR;   // Portuguese (factory default)

  // TFT / pins
  // pinMode(TFT_BL, OUTPUT);
  // digitalWrite(TFT_BL, HIGH);
  // tft.init();
  // tft.setRotation(0);

  // If you serve static HTML from the SD card, DO NOT
  // forget to register the i18n dictionary endpoint:
  //
  //   server.on("/i18n.js", handleI18nJS);  // REQUIRED
  //   server.on("/",        handleLoginRoute);
  //   server.on("/list.html", handleListHTML);
  //   server.on("/v.html",    handleListHTML2);
  //   server.on("/edit",      handleEditFile);
  //   server.on("/delete",    handleDeleteFile);
  //   server.on("/doLogin",   handleDoLogin);
  //   server.on("/logout",    handleLogout);
  //   server.on("/listJSON",  handleListJSON);
  //   server.on("/upload",    HTTP_POST, handleUpload);
  //   server.begin();
}

void loop() {
  // server.handleClient();
  // if (forceRedraw) { forceRedraw = false; /* redraw your screen */ }
  delay(20);
}
```

> **Note on `allconfigs.h`:** The old sketch template included `#include "allconfigs.h"` first. That file is **OPTIONAL** since v1.1. `allfuc.h` uses `__has_include("allconfigs.h")` to load it only when present. Put custom pin macros / passwords there if you need them; otherwise the library uses reasonable CYD defaults.

---

## 🌍 Multi-Language System (NEW in v1.1)

### Two layers, one single toggle.

Everything (TFT, HTTP text, *and* the static SD card web pages) is translated by setting one global variable once in `setup()`:

```cpp
GlobalLanguage = LANG_EN_US;   // or LANG_PT_BR
```

### (A) Source-level translation: `tr(pt, en)`

Wrap every literal string that should change languages with the `tr()` inline helper
(defined in [`lang.h`](src/lang.h)):

```cpp
Serial.println(tr("Carregando...", "Loading..."));
tft.drawCentreString(tr("CONTA", "ACCOUNT"), 120, 10, 4);
server.send(200, "text/plain", tr("Operacao concluida!", "Operation complete!"));
```

> **Rule of thumb:** *first* argument = Portuguese text, *second* = English text.

All 170+ strings inside `allfuc.h` (TFT WiFi scan, on-screen keyboard, loading bar, weather
description, footer, weekday names, HTTP route handlers) and all responses in the reference
sketch are already wrapped for you.

### (B) Client-side translation for static SD HTML files

The ESP32 serves pages like `/login.html`, `/list.html`, `/v.html`, `/edit.html` straight
from the SD card using `server.streamFile()` (**zero** server-side RAM cost, no placeholder
replacement). Translation of these files is done entirely in the browser:

1. The library exposes the **`/i18n.js`** endpoint (implemented by `handleI18nJS` in [`allfuc.h#L153-L251`](src/allfuc.h#L153-L251)).
2. Register it once in your `setup()`:
   ```cpp
   server.on("/i18n.js", handleI18nJS);
   ```
3. In every HTML file on the SD, add this one script tag in `<head>`:
   ```html
   <script src="/i18n.js"></script>
   ```
4. Mark every translatable element with `data-i18n="key"` and the dictionary is applied
   automatically on `DOMContentLoaded`:
   ```html
   <title data-i18n="login.title">Login - 2FATouch</title>
   <h2    data-i18n="login.subtitle">Enter your password</h2>
   ```
5. For **dynamic JS strings** (prompts, alerts, confirms, innerHTML): declare a hook
   function that the i18n loader will call for you:
   ```html
   <script>
   window.applyCustomI18N = function(tr, lang) {
     // re-assign your prompt/confirm/error messages here using tr("key")
     window._saveMsg  = tr("edit.saving");
     window._savedOk  = tr("edit.saved");
   };
   async function salvarArquivo() {
     msg.innerText = window._saveMsg;   // "Salvando..."  or  "Saving..."
     // ...
   }
   </script>
   ```

Predefined dictionary keys available out-of-the-box in `handleI18nJS`:

| Prefix | For HTML file |
|---|---|
| `login.*`  | `SD/login.html` — title, labels, button, error messages, `realizarLogin()` inline JS |
| `list.*`   | `SD/list.html`  — header, buttons, table headers, overlay, prompts, browser JS |
| `v.*`      | `SD/v.html`     — sidebar, player, upload, confirms, timeout, playback errors |
| `edit.*`   | `SD/edit.html`  — title, "Editing:" prefix, placeholder, save/cancel buttons, load/save messages |

Reference translated HTML templates are provided with the library (copy the files from the reference firmware's `SD/` folder onto a FAT32-formatted SD card for the CYD).

---

## 🛣️ Available HTTP Route Handlers

All of these are implemented in [`allfuc.h`](src/allfuc.h) — just `server.on("path", handlerName)` them in your setup:

| Handler | Path suggestion | Description |
|---|---|---|
| `handleI18nJS`     | `/i18n.js`   | **Required for SD pages i18n** — serves the JS dictionary + auto-applier |
| `handleLoginRoute` | `/`, `/login`, `/login.html` | Serves `SD/login.html` from the SD card (stream) |
| `handleDoLogin`    | `/doLogin`   | Accepts POST user/pass, checks SHA256 hash on SD, sets cookie |
| `handleLogout`     | `/logout`    | Clears the session cookie |
| `handleLogoutCustom` | `/leave`   | Vault-mode logout |
| `handleListHTML`   | `/list`, `/list.html` | Serves the SD file manager page (stream) |
| `handleListHTML2`  | `/v`, `/v.html` | Serves the media player page (stream) |
| `handleEditFile`   | `/edit`      | Opens `SD/edit.html?file=…` (editor for text/HTML/JSON) |
| `handleDeleteFile` | `/delete`    | Removes a file from the SD card |
| `handleListJSON`   | `/listJSON`  | Scans SD and returns a JSON tree (AJAX for the file manager) |
| `handleUpload`     | `/upload`    | HTTP POST multipart upload to the SD |
| `saveXARQ` (sketch) | `/saveXARQ` | Saves editor content back to SD path (implemented in reference sketch) |

---

## 🎨 TFT UI Functions

All in [`allfuc.h`](src/allfuc.h) and already internationalized:

| Function | What it draws |
|---|---|
| `drawLoadingCreeper(cx, cy, size)` | Scalable pixel-art Creeper face (used on boot) |
| `drawLoadingScreen(percent)`       | Boot loader with progress bar, % counter, bilingual messages |
| `iniciarScanWiFiTFT()`             | Starts the WiFi network scan for the TFT UI |
| `drawWiFiScanScreen()`             | Touchable "SELECT WI-FI" page with SSID list + "Back to menu" button |
| `drawWiFiKeyboardScreen()`         | On-screen touch keyboard for WPA2 passwords |
| `atualizarCaixaSenhaTFT()`         | Updates the masked password box while typing |
| `carregarTelaMeteorologia()`       | Weather widget (temp, humidity, wind, icon + description, weekday) |
| `getFooter()`                      | Shared HTML `</footer>` for HTTP pages — translated |
| `classificarVento(kmh)`            | Beaufort scale in PT or EN (ex.: "Brisa Leve" / "Light Breeze") |
| `get_weather_description(code)`    | OpenWeather id → Portuguese/English human description |
| `drawInfo(...)`                    | Full 2FA TOTP token screen — translated weekday names |

---

## 🧩 Expected Globals (Declare these in your sketch)

The following are **referenced but not declared** by the core library (so you can customise
their constructors / pins). Copy the block from the quick-start example:

| Symbol | Type | Used for |
|---|---|---|
| `tft`           | `TFT_eSPI`     | Every TFT draw function |
| `server`        | `WebServer`    | All HTTP route handlers |
| `forceRedraw`   | `bool`         | TFT state machine (set to `true` to repaint) |
| `displayMode`   | `int`          | 0 = menu, 1 = tokens, … (match your sketch's modes) |
| `wifiScanPage`  | `int`          | Current page of the WiFi scan results list |
| `TFT_BL`, `PIN_RED`, `PIN_GREEN`, `PIN_BLUE` | integer macros | Backlight + RGB LED pins, used by `ligarTela()` / `desligarTela()` |

You can either `#define` these pins in your sketch or put them into an optional
`allconfigs.h` file (picked up automatically by `__has_include`).

---

## ✅ Compatibility & Troubleshooting

### "`multiple definition of …` at link time" (ld error)
Fixed in v1.1:
- Language symbols are now in a tiny separate `lang.h` + `lang.cpp` translation unit.
- `lang.cpp` only includes `lang.h` (not the heavy `allfuc.h`), so there are no duplicate
  symbols when `allfuc.h` is header-only.
- No circular includes inside `allfuc.h`.

### "`WebServer::send(int, String)` — no matching function"
Always provide the 3-argument form `server.send(code, "mime/type", content)`.
See the reference sketch: all error routes use `"text/plain"` as the second argument,
e.g. `server.send(403, "text/plain", tr("Negado", "Denied"));`.

### Static SD pages still show Portuguese when `GlobalLanguage = LANG_EN_US`
Checklist:
1. Did you register `server.on("/i18n.js", handleI18nJS);`?
2. Does the HTML file contain `<script src="/i18n.js"></script>` in `<head>`?
3. Did you actually flash the **new** HTML files to the physical SD card?
   (Old files on the SD are not touched by the firmware upload.)

---

## 📚 Example Sketch

Open the fully commented bundled example:
> **Arduino IDE** → **File → Examples → 2FATouchCore → 2FATouchCoreExample**

It shows:
- How to toggle `GlobalLanguage`
- Where to place `tr("…", "…")` calls in your own code
- Default pin macros
- Full server route registration for the SD pages

---

## 🔗 Links / Community

- **Repository (issues / PRs welcome):** [github.com/Annabel369/2FATouchCore](https://github.com/Annabel369/2FATouchCore)
- **Reference firmware (`2FATouch.ino`):** [Annabel369/2FATouch](https://github.com/Annabel369/2FATouch)
- **Hardware:** ESP32 CYD (ESP32-2432S028R) with ILI9341 TFT + XPT2046 touch + microSD slot

---

## 📄 License

Released under the **MIT License**. See [`LICENSE`](LICENSE) file.
