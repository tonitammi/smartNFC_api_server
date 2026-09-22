#ifndef SMARTNFC_API_H
#define SMARTNFC_API_H

#include <Arduino.h>
#if defined(ESP32)
  #include <WiFi.h>
  #include <WebServer.h>
#elif defined(ESP8266)
  #include <ESP8266WiFi.h>
  #include <ESP8266WebServer.h>
  using WebServer = ESP8266WebServer;
#endif

#include <map>
#include <functional>
#include <ArduinoJson.h>

// Handler for endpoints without payload (e.g., GET requests)
typedef std::function<void()> SimpleApiHandler;

// Handler for endpoints receiving a parsed JSON body (e.g., POST/PUT requests)
typedef std::function<void(const JsonDocument& doc)> JsonApiHandler;

class SmartNFC_API {
public:
    SmartNFC_API(uint16_t port = 80);

    void begin();
    void handleClient();
    void printServerUrl();

    // API Key management
    void setGlobalApiKey(const String& key);
    void setRouteApiKey(const String& path, const String& key);

    // GET endpoints
    void addGetEndpoint(const String& path, SimpleApiHandler handler);

    // POST endpoints (with or without JSON payload)
    void addPostEndpoint(const String& path, SimpleApiHandler handler);
    void addPostEndpoint(const String& path, JsonApiHandler handler);

    // Helper methods for JSON responses
    void sendJsonResponse(int statusCode, const String& rawJson);
    void sendJsonResponse(int statusCode, const JsonDocument& doc);

private:
    WebServer _server;
    uint16_t _port;
    String _globalApiKey;
    std::map<String, String> _routeApiKeys;

    bool isAuthorized(const String& path);
    void handleCorsAndOptions();
};

#endif
