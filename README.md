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
```

### Arduino IDE
Copy `SmartNFC_API.h` and `SmartNFC_API.cpp` directly into your sketch folder or import as a custom zip library.

---

## Quick Start Example

```cpp
#include <Arduino.h>
#include <WiFi.h>
#include "SmartNFC_API.h"

SmartNFC_API api(80);

void setup() {
    Serial.begin(115200);
    WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");
    while (WiFi.status() != WL_CONNECTED) { delay(500); }

    // Set a route-specific API key
    api.setRouteApiKey("/api/action", "YourSecretKey123");

    // Define a GET endpoint returning JSON
    api.addGetEndpoint("/api/status", []() {
        JsonDocument doc;
        doc["status"] = "online";
        doc["temp"] = 23.4;
        api.sendJsonResponse(200, doc);
    });

    // Define a protected POST endpoint expecting a JSON body
    api.addPostEndpoint("/api/action", [](const JsonDocument& doc) {
        String state = doc["state"].as<String>();
        
        JsonDocument res;
        res["status"] = "success";
        res["executed_state"] = state;
        api.sendJsonResponse(200, res);
    });

    api.begin();
    api.printServerUrl(); // Outputs: [SmartNFC_API] API Server URL: http://192.168.1.100
}

void loop() {
    api.handleClient();
}
```

---

## Authentication Usage

Clients can authenticate requests to protected endpoints using either method:

1. **HTTP Header:**
   ```http
   X-API-Key: YourSecretKey123
   ```
2. **Query Parameter:**
   ```http
   POST http://192.168.1.100/api/action?api_key=YourSecretKey123
   ```

---

## License

MIT License. Free for personal and commercial use.
