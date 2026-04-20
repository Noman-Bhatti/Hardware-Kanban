#include "config.h"
#include "display_utils.h"
#include "wifi_client.h"

// Tracks last known state to avoid unnecessary screen refreshes
String lastTitle      = "";
String lastColumnName = "";
bool   lastPaired     = false;

unsigned long lastPollTime = 0;

void setup() {
  Serial.begin(115200);
  initDisplay();

  renderMessage("Connecting...", "WiFi");

  if (!connectWifi()) {
    renderMessage("WiFi failed", "Check config.h");
    return;
  }

  // Wake the device — resets pairing, gets fresh PIN
  renderMessage("Waking device...", "");
  String pin = wakeDevice();

  if (pin == "") {
    renderMessage("Wake failed", "Check serial");
    return;
  }

  renderPin(pin);
  Serial.println("PIN displayed: " + pin);
}

void loop() {
  // Poll every 10 seconds while unpaired, every 5 min when paired
  unsigned long interval = lastPaired ? POLL_INTERVAL_MS : 10000;

  if (millis() - lastPollTime < interval) return;
  lastPollTime = millis();

  String title, columnName;
  int status = pollStatus(title, columnName);

  if (status == -1) {
    Serial.println("Poll failed");
    return;
  }

  if (status == 0) {
    // Still waiting for pairing — no screen update needed
    Serial.println("Waiting for pair...");
    lastPaired = false;
    return;
  }

  // Paired
  if (!lastPaired) {
    // Just paired — force render
    lastPaired     = true;
    lastTitle      = "";
    lastColumnName = "";
  }

  if (title == "" && columnName == "") {
    // Paired but no task available
    if (lastTitle != "NO_TASK") {
      renderMessage("No tasks", "All done!");
      lastTitle = "NO_TASK";
    }
    return;
  }

  // Only refresh screen if something changed
  if (title != lastTitle || columnName != lastColumnName) {
    renderTask(title, columnName);
    lastTitle      = title;
    lastColumnName = columnName;
    Serial.println("Rendered: " + title + " [" + columnName + "]");
  }
}