#include "SmartNFC_API.h"

SmartNFC_API::SmartNFC_API(uint16_t port) : _server(port), _port(port), _globalApiKey("") {}

void SmartNFC_API::begin() {
    // Handle CORS preflight (OPTIONS) requests and 404 Not Found globally
    _server.onNotFound([this]() {
        if (_server.method() == HTTP_OPTIONS) {
            handleCorsAndOptions();
        } else {
            sendJsonResponse(404, "{\"status\":\"error\",\"message\":\"Not Found\"}");
        }
    });

    _server.begin();
    Serial.println("[SmartNFC_API] Server started.");
}

void SmartNFC_API::handleClient() {
    _server.handleClient();
}

void SmartNFC_API::printServerUrl() {
    if (WiFi.status() == WL_CONNECTED) {
        String url = "http://" + WiFi.localIP().toString();
        if (_port != 80) {
            url += ":" + String(_port);
        }
        Serial.print("[SmartNFC_API] API Server URL: ");
        Serial.println(url);
    } else {
        Serial.println("[SmartNFC_API] Wi-Fi not connected. Cannot print server URL.");
    }
}

void SmartNFC_API::setGlobalApiKey(const String& key) {
    _globalApiKey = key;
}

void SmartNFC_API::setRouteApiKey(const String& path, const String& key) {
    _routeApiKeys[path] = key;
}

bool SmartNFC_API::isAuthorized(const String& path) {
    String requiredKey = "";

    // 1. Check for route-specific API key
    if (_routeApiKeys.find(path) != _routeApiKeys.end()) {
        requiredKey = _routeApiKeys[path];
    } 
    // 2. Fall back to global API key
    else if (_globalApiKey.length() > 0) {
        requiredKey = _globalApiKey;
    }

    // If no key is set, access is granted
    if (requiredKey.length() == 0) {
        return true;
    }

    // Check key from HTTP header ("X-API-Key") or query param ("?api_key=")
    String providedKey = "";
    if (_server.hasHeader("X-API-Key")) {
        providedKey = _server.header("X-API-Key");
    } else if (_server.hasArg("api_key")) {
        providedKey = _server.arg("api_key");
    }

    return (providedKey.length() > 0 && providedKey.equals(requiredKey));
}

void SmartNFC_API::handleCorsAndOptions() {
    _server.sendHeader("Access-Control-Allow-Origin", "*");
    _server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS, PUT, DELETE");
    _server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-API-Key");
    _server.send(204); // No Content
}

void SmartNFC_API::sendJsonResponse(int statusCode, const String& rawJson) {
    _server.sendHeader("Access-Control-Allow-Origin", "*");
    _server.sendHeader("Access-Control-Allow-Headers", "Content-Type, X-API-Key");
    _server.send(statusCode, "application/json", rawJson);
}

void SmartNFC_API::sendJsonResponse(int statusCode, const JsonDocument& doc) {
    String output;
    serializeJson(doc, output);
    sendJsonResponse(statusCode, output);
}

void SmartNFC_API::addGetEndpoint(const String& path, SimpleApiHandler handler) {
    _server.on(path.c_str(), HTTP_GET, [this, path, handler]() {
        if (!isAuthorized(path)) {
            sendJsonResponse(401, "{\"status\":\"error\",\"message\":\"Unauthorized: Invalid or missing API Key\"}");
            return;
        }
        handler();
    });
}

void SmartNFC_API::addPostEndpoint(const String& path, SimpleApiHandler handler) {
    _server.on(path.c_str(), HTTP_POST, [this, path, handler]() {
        if (!isAuthorized(path)) {
            sendJsonResponse(401, "{\"status\":\"error\",\"message\":\"Unauthorized: Invalid or missing API Key\"}");
            return;
        }
        handler();
    });
}

void SmartNFC_API::addPostEndpoint(const String& path, JsonApiHandler handler) {
    _server.on(path.c_str(), HTTP_POST, [this, path, handler]() {
        if (!isAuthorized(path)) {
            sendJsonResponse(401, "{\"status\":\"error\",\"message\":\"Unauthorized: Invalid or missing API Key\"}");
            return;
        }

        // Parse incoming JSON body
        String body = _server.arg("plain");
        JsonDocument doc;
        DeserializationError error = deserializeJson(doc, body);

        if (error) {
            JsonDocument errDoc;
            errDoc["status"] = "error";
            errDoc["message"] = "Invalid JSON payload";
            errDoc["details"] = error.c_str();
            sendJsonResponse(400, errDoc);
            return;
        }

        handler(doc);
    });
}
