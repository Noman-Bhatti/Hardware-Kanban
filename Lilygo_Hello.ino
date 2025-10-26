#include <GxEPD2_BW.h>

GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(
  GxEPD2_213_BN(5, 17, 16, 4)  // CS, DC, RST, BUSY pins
);

void setup() {
  display.init(115200);
  display.setRotation(1);
  display.fillScreen(GxEPD_WHITE);
  display.setTextColor(GxEPD_BLACK);
  display.setTextSize(3);
  display.setCursor(10, 30);
  display.print("Holy shit!");
  display.setCursor(10, 60);
  display.print("It works!!");
  display.display();
}

void loop() {}