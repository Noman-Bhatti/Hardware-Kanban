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

bool fetchTask(String &title, String &columnName) {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();  // Skip SSL cert verification for now

  HTTPClient http;
  String url = String(API_BASE_URL) + "/api/devices/" + DEVICE_ID + "/task";
  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " DEVICE_TOKEN);
  http.addHeader("Content-Type", "application/json");

  int code = http.GET();
  Serial.println("GET task status: " + String(code));

  if (code == 200) {
    JsonDocument doc;
    DeserializationError err = deserializeJson(doc, http.getString());
    if (!err) {
      title      = doc["title"].as<String>();
      columnName = doc["column_name"].as<String>();
      http.end();
      return true;
    }
    Serial.println("JSON parse error: " + String(err.c_str()));
  } else {
    Serial.println("Response: " + http.getString());
  }

  http.end();
  return false;
}

bool updateColumn(String columnName) {
  if (WiFi.status() != WL_CONNECTED) return false;

  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;
  String url = String(API_BASE_URL) + "/api/devices/" + DEVICE_ID + "/task";
  http.begin(client, url);
  http.addHeader("Authorization", "Bearer " DEVICE_TOKEN);
  http.addHeader("Content-Type", "application/json");

  JsonDocument doc;
  doc["column_name"] = columnName;
  String body;
  serializeJson(doc, body);

  int code = http.PATCH(body);
  Serial.println("PATCH column status: " + String(code));
  Serial.println("Response: " + http.getString());

  http.end();
  return (code == 200);
}