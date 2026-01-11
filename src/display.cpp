#include "display.h"
#include "theme.h"
#include "config.h"
#include <Adafruit_GFX.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

void initDisplay() {
    tft.init(240, 320);
    tft.setRotation(1);
    tft.setTextWrap(true);
    tft.invertDisplay(false);
    applyTheme();
}

void applyTheme() {
    Theme current = getCurrentTheme();
    tft.fillScreen(current.bg);
    tft.setTextColor(current.fg);
    tft.setCursor(0, 0);
}

void clearScreen() {
    Theme current = getCurrentTheme();
    tft.fillScreen(current.bg);
    tft.setCursor(0, 0);
}

void printLine(String s) {
    tft.println(s);
    Serial.println(s);
}
void screensaver() {
    screenLocked = true;
    tft.fillScreen(ST77XX_BLACK);

    const int width = 320;
    const int height = 230;

    tft.startWrite();
    static uint16_t line[320]; 

    int offset = 0;
    Serial.println("Press ENTER to exit...");
    while (true) {
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint16_t color = ((x + y + offset) % 60 < 30) ? ST77XX_CYAN : ST77XX_BLUE;
                line[x] = color;
            }
            tft.setAddrWindow(0, y, width, 1);
            tft.writePixels(line, width);
        }

        offset++;
        if (offset > 60) offset = 0;

        tft.endWrite();

        tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
        tft.setTextSize(1);
        tft.setCursor(5, 230);
        tft.print("Press ENTER to exit...");
        

        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') break;
        }

        tft.startWrite();
        delay(50);
    }

    tft.endWrite();
    applyTheme();
    screenLocked = false;
    clearScreen();
}


void showLogo() {
    printLine(",--.   ,--.,--.,--.  ,--.,--.    ,-----.  ,---.  ");
    printLine("|   `.'   ||  ||  ,'.|  ||  |   '  .-.  ''   .-' ");
    printLine("|  |'.'|  ||  ||  |' '  ||  |   |  | |  |`.  `-. ");
    printLine("|  |   |  ||  ||  | `   ||  |   '  '-'  '.-'    |");
    printLine("`--'   `--'`--'`--'  `--'`--'    `-----' `-----' ");
    printLine("");
    printLine(OS_VERSION);
    printLine("Check: https://github.com/VuqarAhadli");
}