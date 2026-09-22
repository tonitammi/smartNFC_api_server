# SmartNFC_API for ESP32 / ESP8266

A lightweight, wrapper library for ESP32 and ESP8266 microcontrollers designed to quickly expose a JSON REST API for **SmartNFC Smart Actions** and other IoT web integrations.

## Features

- **Automated CORS Handling:** Built-in support for CORS headers (`Access-Control-Allow-Origin: *`) and Preflight (`OPTIONS`) requests for web/mobile client integrations.
- **API Key Security:** Protect all routes globally with `setGlobalApiKey()` or secure individual endpoints with `setRouteApiKey()`. Supports API keys via HTTP Header (`X-API-Key`) or URL Query Parameter (`?api_key=`).
- **Seamless JSON Parsing:** Powered by `ArduinoJson` v7, POST/PUT JSON payloads are parsed automatically and passed directly to endpoint callbacks.
- **Helper Utilities:** Easily print the active server IP address and URL with `printServerUrl()`.

---

## Dependencies

This library requires **ArduinoJson** (v7.x or newer).

- **PlatformIO:** Add `bblanchon/ArduinoJson` to your `platformio.ini` dependencies.
- **Arduino IDE:** Install `ArduinoJson` via Library Manager (`Tools` -> `Manage Libraries...`).

---

## Installation

### PlatformIO
Add the source files (`SmartNFC_API.h` and `SmartNFC_API.cpp`) to your project's `lib/SmartNFC_API/` directory or include this repository URL in your `platformio.ini`:

```ini
lib_deps =
    bblanchon/ArduinoJson@^7.0.0
