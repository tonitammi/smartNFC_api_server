#include <Arduino.h>
#include <WiFi.h>
#include "SmartNFC_API.h"

const char* ssid = "YOUR_WIFI_SSID";
const char* password = "YOUR_WIFI_PASSWORD";

SmartNFC_API api(80);

void setup() {
    Serial.begin(9600);
    
    // Connect to Wi-Fi
    WiFi.begin(ssid, password);
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(".");
    }
    Serial.println("\nWi-Fi connected!");

    // --- API KEY CONFIGURATION ---
    
    // Option A: Set a global key for all routes
    // api.setGlobalApiKey("MyGlobalSecretKey123");

    // Option B: Set a route-specific key (overrides global key for this route)
    api.setRouteApiKey("/api/light", "SmartNFC_SecretKey_987");

    // --- ENDPOINT DEFINITIONS ---

    // 1. Open GET Endpoint (e.g., retrieving sensor readings)
    api.addGetEndpoint("/api/status", []() {
        JsonDocument doc;
        doc["status"] = "ok";
        doc["uptime_ms"] = millis();
        doc["temperature"] = 22.5;

        api.sendJsonResponse(200, doc);
    });

    // 2. Protected POST Endpoint with JSON Payload (e.g., SmartNFC Smart Action trigger)
    // Example JSON payload: {"state": "ON", "brightness": 80}
    api.addPostEndpoint("/api/light", [](const JsonDocument& doc) {
        if (doc.containsKey("state")) {
            String state = doc["state"].as<String>();
            int brightness = doc["brightness"] | 100; // Default to 100 if omitted

            Serial.printf("Light state updated: %s, brightness: %d%%\n", state.c_str(), brightness);

            JsonDocument res;
            res["status"] = "success";
            res["applied_state"] = state;
            res["applied_brightness"] = brightness;

            api.sendJsonResponse(200, res);
        } else {
            JsonDocument err;
            err["status"] = "error";
            err["message"] = "Missing required 'state' field";
            api.sendJsonResponse(400, err);
        }
    });

    // Start server
    api.begin();

    // Print server URL to Serial Monitor
    api.printServerUrl();
}

void loop() {
    api.handleClient();
}
