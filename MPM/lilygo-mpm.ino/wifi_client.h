#pragma once

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

// WiFi ---------------------------------

bool connectWifi() {
  Serial.print("Connecting to WiFi");
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);

  int attempts = 0;
  while (WiFi.status() != WL_CONNECTED && attempts < 20) {
    delay(500);
    Serial.print(".");
    attempts++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected: " + WiFi.localIP().toString());
    return true;
  }

  Serial.println("\nWiFi failed.");
  return false;
}

// Shared HTTP helper ---------------------------------

WiFiClientSecure _secureClient;

HTTPClient _beginRequest(String path) {
  _secureClient.setInsecure();
  HTTPClient http;
  http.begin(_secureClient, String(API_BASE_URL) + path);
  http.addHeader("Authorization", "Bearer " DEVICE_TOKEN);
  http.addHeader("Content-Type", "application/json");
  return http;
}

// Wake - call on every boot  ---------------------------------
// Returns PIN string, or "" on failure

String wakeDevice() {
  HTTPClient http = _beginRequest("/api/devices/" DEVICE_ID "/wake");

  int code = http.POST("");
  Serial.println("Wake status: " + String(code));

  if (code == 200) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, http.getString());
    if (!err) {
      String pin = doc["data"]["pin"].as<String>();
      http.end();
      return pin;
    }
  } else {
    Serial.println("Wake response: " + http.getString());
  }

  http.end();
  return "";
}

// Status poll ---------------------------------
// Returns: 0 = not paired, 1 = paired + task loaded, -1 = error

int pollStatus(String &title, String &columnName) {
  HTTPClient http = _beginRequest("/api/devices/" DEVICE_ID "/status");

  int code = http.GET();
  Serial.println("Status poll: " + String(code));

  if (code == 200) {
    String raw = http.getString();
    Serial.println("Status response: " + raw);
    http.end();

    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, raw);
    if (err) return -1;

    bool paired = doc["paired"] | false;
    if (!paired) return 0;

    // paired but no task yet (all tasks done or none assigned)
    if (doc["task"].isNull()) {
      title = "";
      columnName = "";
      return 1;
    }

    title      = doc["task"]["title"].as<String>();
    columnName = doc["task"]["column_name"].as<String>();
    return 1;
  }

  http.end();
  return -1;
}

// Update column (BLE phase) ---------------------------------

bool updateColumn(String columnName) {
  HTTPClient http = _beginRequest("/api/devices/" DEVICE_ID "/task");

  JsonDocument doc;
  doc["column_name"] = columnName;
  String body;
  serializeJson(doc, body);

  int code = http.PATCH(body);
  Serial.println("PATCH column: " + String(code));

  http.end();
  return (code == 200);
}