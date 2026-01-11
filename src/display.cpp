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
    tft.setCursor(5, 0);
}

void clearScreen() {
    Theme current = getCurrentTheme();
    tft.fillScreen(current.bg);
    tft.setCursor(5, 0);
}

void printLine(String s) {
    int16_t y = tft.getCursorY();  
    tft.setCursor(5, y);            
    tft.println(s);                 
    Serial.println(s);              
}


void screensaver(int mode) {
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
                uint16_t color = ST77XX_BLACK;
                
                switch(mode) {
                    case 1:
                        color = ((x + y + offset) % 60 < 30) ? ST77XX_CYAN : ST77XX_BLUE;
                        break;
                        
                    case 2:
                        {
                            int hue = (x + y + offset) % 256;
                            if (hue < 43) color = ST77XX_RED;
                            else if (hue < 85) color = ST77XX_ORANGE;
                            else if (hue < 128) color = ST77XX_YELLOW;
                            else if (hue < 171) color = ST77XX_GREEN;
                            else if (hue < 214) color = ST77XX_CYAN;
                            else color = ST77XX_MAGENTA;
                        }
                        break;
                        
                    case 3:
                        {
                            int gridX = (x + offset) % 128;
                            int gridY = (y + offset / 2) % 128;
                            
                            if (gridX < 32 && gridY < 32) color = ST77XX_RED;
                            else if (gridX >= 32 && gridX < 64 && gridY < 32) color = ST77XX_GREEN;
                            else if (gridX >= 64 && gridX < 96 && gridY < 32) color = ST77XX_BLUE;
                            else if (gridX >= 96 && gridY < 32) color = ST77XX_YELLOW;
                            else if (gridX < 32 && gridY >= 32 && gridY < 64) color = ST77XX_CYAN;
                            else if (gridX >= 32 && gridX < 64 && gridY >= 32 && gridY < 64) color = ST77XX_MAGENTA;
                            else if (gridX >= 64 && gridX < 96 && gridY >= 32 && gridY < 64) color = ST77XX_WHITE;
                            else if (gridX >= 96 && gridY >= 32 && gridY < 64) color = ST77XX_ORANGE;
                            else if (gridX < 32 && gridY >= 64 && gridY < 96) color = 0xFC9F;
                            else if (gridX >= 32 && gridX < 64 && gridY >= 64 && gridY < 96) color = 0x780F;
                            else if (gridX >= 64 && gridX < 96 && gridY >= 64 && gridY < 96) color = 0x8200;
                            else if (gridX >= 96 && gridY >= 64 && gridY < 96) color = 0xC618;
                            else if (gridX < 32 && gridY >= 96) color = 0x4208;
                            else if (gridX >= 32 && gridX < 64 && gridY >= 96) color = 0x000F;
                            else if (gridX >= 64 && gridX < 96 && gridY >= 96) color = 0x0410;
                            else color = 0x7BE0;
                        }
                        break;
                        
                    case 4:
                        {
                            int val = (x * x / 200 + y * y / 200 + offset) % 128;
                            if (val < 32) color = ST77XX_BLUE;
                            else if (val < 64) color = ST77XX_CYAN;
                            else if (val < 96) color = ST77XX_MAGENTA;
                            else color = ST77XX_RED;
                        }
                        break;
                        
                    case 5:
                        {
                            int blockSize = 20;
                            bool isBlock = ((x / blockSize) + (y / blockSize) + (offset / 10)) % 2 == 0;
                            color = isBlock ? ST77XX_CYAN : ST77XX_MAGENTA;
                        }
                        break;
                        
                    case 6:
                        {
                            int heat = ((height - y) * 2 + (x + offset) % 40) % 100;
                            if (heat > 70) color = ST77XX_YELLOW;
                            else if (heat > 40) color = ST77XX_ORANGE;
                            else if (heat > 20) color = ST77XX_RED;
                            else color = ST77XX_BLACK;
                        }
                        break;
                        
                    case 7:
                        {
                            int starPos = (x * 17 + y * 13 + offset) % 500;
                            color = (starPos < 3) ? ST77XX_WHITE : ST77XX_BLACK;
                        }
                        break;
                        
                    default:
                        color = ((x + y + offset) % 60 < 30) ? ST77XX_CYAN : ST77XX_BLUE;
                        break;
                }
                
                line[x] = color;
            }
            tft.setAddrWindow(0, y, width, 1);
            tft.writePixels(line, width);
        }
        
        if (mode == 3 || mode == 6) {
            offset += 3;
        } else if (mode == 7) {
            offset += 5;
        } else {
            offset += 2;
        }
        
        if (offset > 500) offset = 0;
        
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