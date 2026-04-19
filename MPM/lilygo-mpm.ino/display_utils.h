#pragma once

#include <GxEPD2_BW.h>
#include <Fonts/FreeMonoBold9pt7b.h>
#include <Fonts/FreeMono9pt7b.h>

// Display instance — matches your working Hello World exactly
GxEPD2_BW<GxEPD2_213_BN, GxEPD2_213_BN::HEIGHT> display(
  GxEPD2_213_BN(5, 17, 16, 4)  // CS, DC, RST, BUSY
);

// Screen dimensions (rotation 1 = landscape)
#define SCREEN_W 250
#define SCREEN_H 122
#define PADDING  8
#define BADGE_H  22

// Helpers ------------------------------------------------

// Draws the column badge at bottom of screen (inverted rectangle)
void drawBadge(String label) {
  int badgeY = SCREEN_H - BADGE_H;

  // Black filled rectangle
  display.fillRect(PADDING, badgeY, SCREEN_W - (PADDING * 2), BADGE_H, GxEPD_BLACK);

  // White text centered inside badge
  display.setFont(&FreeMono9pt7b);
  display.setTextColor(GxEPD_WHITE);

  int16_t x1, y1;
  uint16_t tw, th;
  display.getTextBounds(label, 0, 0, &x1, &y1, &tw, &th);

  int textX = (SCREEN_W - tw) / 2;
  int textY = badgeY + BADGE_H - 6;
  display.setCursor(textX, textY);
  display.print(label);
}

// Word-wraps title text, returns number of lines drawn
void drawTitle(String title) {
  display.setFont(&FreeMonoBold9pt7b);
  display.setTextColor(GxEPD_BLACK);

  // Max chars per line at this font/size on 250px wide screen
  const int charsPerLine = 20;
  int y = PADDING + 14;  // first baseline

  while (title.length() > 0 && y < (SCREEN_H - BADGE_H - 6)) {
    String line;
    if ((int)title.length() <= charsPerLine) {
      line = title;
      title = "";
    } else {
      // Break at last space before limit
      int breakAt = charsPerLine;
      for (int i = charsPerLine; i > 0; i--) {
        if (title.charAt(i) == ' ') { breakAt = i; break; }
      }
      line = title.substring(0, breakAt);
      title = title.substring(breakAt + 1);
    }
    display.setCursor(PADDING, y);
    display.print(line);
    y += 18;
  }
}

// Public functions ------------------------------------------------

void initDisplay() {
  display.init(115200);
  display.setRotation(1);
}

void renderTask(String title, String columnName) {
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    drawTitle(title);
    drawBadge(columnName);
  } while (display.nextPage());
}

void renderMessage(String line1, String line2 = "") {
  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);
    display.setFont(&FreeMono9pt7b);
    display.setTextColor(GxEPD_BLACK);
    display.setCursor(PADDING, 30);
    display.print(line1);
    if (line2.length() > 0) {
      display.setCursor(PADDING, 54);
      display.print(line2);
    }
  } while (display.nextPage());
}