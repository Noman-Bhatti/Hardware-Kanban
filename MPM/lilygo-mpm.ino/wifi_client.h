#pragma once

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include "config.h"

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

String wakeDevice() {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, String(API_BASE_URL) + "/api/devices/" + DEVICE_ID + "/wake");
  http.addHeader("Authorization", "Bearer " DEVICE_TOKEN);
  http.addHeader("Content-Type", "application/json");

  int code = http.POST("");
  Serial.println("Wake status: " + String(code));

  if (code == 200) {
    JsonDocument doc;
    String raw = http.getString();
    Serial.println("Wake response: " + raw);
    DeserializationError err = deserializeJson(doc, raw);
    if (!err) {
      http.end();
      return doc["pin"].as<String>();
    }
  } else {
    Serial.println("Wake response: " + http.getString());
  }

  http.end();
  return "";
}

int pollStatus(String &title, String &columnName) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, String(API_BASE_URL) + "/api/devices/" + DEVICE_ID + "/status");
  http.addHeader("Authorization", "Bearer " DEVICE_TOKEN);
  http.addHeader("Content-Type", "application/json");

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

bool updateColumn(String columnName) {
  WiFiClientSecure client;
  client.setInsecure();
  HTTPClient http;
  http.begin(client, String(API_BASE_URL) + "/api/devices/" + DEVICE_ID + "/task");
  http.addHeader("Authorization", "Bearer " DEVICE_TOKEN);
  http.addHeader("Content-Type", "application/json");

  JsonDocument doc;
  doc["column_name"] = columnName;
  String body;
  serializeJson(doc, body);

  int code = http.PATCH(body);
  Serial.println("PATCH column: " + String(code));

  http.end();
  return (code == 200);
}