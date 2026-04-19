#include "config.h"
#include "display_utils.h"
#include "wifi_client.h"

void setup() {
  Serial.begin(115200);
  initDisplay();

  renderMessage("Connecting...", "WiFi");

  if (!connectWifi()) {
    renderMessage("WiFi failed", "Check config.h");
    return;
  }

  renderMessage("Fetching task...", "");

  String title, columnName;
  if (fetchTask(title, columnName)) {
    renderTask(title, columnName);
  } else {
    renderMessage("API failed", "Check serial");
  }
}

void loop() {}