#include "config.h"
#include "display_utils.h"

void setup() {
  Serial.begin(115200);
  initDisplay();

  // Hardcoded test — confirm layout before touching WiFi
  renderTask("Implement login screen", "In Progress");
}

void loop() {}