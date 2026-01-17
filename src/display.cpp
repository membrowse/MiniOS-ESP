#include "display.h"
#include "theme.h"
#include "config.h"
#include <Adafruit_GFX.h>

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);
int16_t currentCursorY = 0;


uint8_t sin8(int angle) {
    return (uint8_t)((sin(angle * M_PI / 128.0) + 1.0) * 127.5);
}

void initDisplay() {
    tft.init(240, 320);
    tft.setRotation(1);
    tft.setTextWrap(true);
    tft.invertDisplay(false);
    applyTheme();
    currentCursorY = 0;
}

void applyTheme() {
    Theme current = getCurrentTheme();
    tft.fillScreen(current.bg);
    tft.setTextColor(current.fg);
    tft.setCursor(5, 0);
    currentCursorY = 0;
}

void clearScreen() {
    Theme current = getCurrentTheme();
    tft.fillScreen(current.bg);
    tft.setCursor(5, 0);
    currentCursorY = 0;
    tft.setTextColor(current.fg, current.bg);
    
}

void printLine(String s) {
    Theme current = getCurrentTheme();
    
    if (currentCursorY > MAX_Y) {
        clearScreen();
    }
    
    tft.setCursor(5, currentCursorY);
    tft.setTextColor(current.fg, current.bg);
    tft.println(s);
    
    currentCursorY = tft.getCursorY();
    
    Serial.println(s);
}


// void screensaver(int mode) {
//     screenLocked = true;
//     tft.fillScreen(ST77XX_BLACK);
//     const int width = 320;
//     const int height = 230;
//     tft.startWrite();
//     static uint16_t line[320];
//     int offset = 0;
//     Serial.println("Press ENTER to exit...");
    
//     while (true) {
//         for (int y = 0; y < height; y++) {
//             for (int x = 0; x < width; x++) {
//                 uint16_t color = ST77XX_BLACK;
                
//                 switch(mode) {
//                     case 1:
//                         color = ((x + y + offset) % 60 < 30) ? ST77XX_CYAN : ST77XX_BLUE;
//                         break;
                        
//                     case 2:
//                         {
//                             int hue = (x + y + offset) % 256;
//                             if (hue < 43) color = ST77XX_RED;
//                             else if (hue < 85) color = ST77XX_ORANGE;
//                             else if (hue < 128) color = ST77XX_YELLOW;
//                             else if (hue < 171) color = ST77XX_GREEN;
//                             else if (hue < 214) color = ST77XX_CYAN;
//                             else color = ST77XX_MAGENTA;
//                         }
//                         break;
                        
//                     case 3:
//                         {
//                             int gridX = (x + offset) % 128;
//                             int gridY = (y + offset / 2) % 128;
                            
//                             if (gridX < 32 && gridY < 32) color = ST77XX_RED;
//                             else if (gridX >= 32 && gridX < 64 && gridY < 32) color = ST77XX_GREEN;
//                             else if (gridX >= 64 && gridX < 96 && gridY < 32) color = ST77XX_BLUE;
//                             else if (gridX >= 96 && gridY < 32) color = ST77XX_YELLOW;
//                             else if (gridX < 32 && gridY >= 32 && gridY < 64) color = ST77XX_CYAN;
//                             else if (gridX >= 32 && gridX < 64 && gridY >= 32 && gridY < 64) color = ST77XX_MAGENTA;
//                             else if (gridX >= 64 && gridX < 96 && gridY >= 32 && gridY < 64) color = ST77XX_WHITE;
//                             else if (gridX >= 96 && gridY >= 32 && gridY < 64) color = ST77XX_ORANGE;
//                             else if (gridX < 32 && gridY >= 64 && gridY < 96) color = 0xFC9F;
//                             else if (gridX >= 32 && gridX < 64 && gridY >= 64 && gridY < 96) color = 0x780F;
//                             else if (gridX >= 64 && gridX < 96 && gridY >= 64 && gridY < 96) color = 0x8200;
//                             else if (gridX >= 96 && gridY >= 64 && gridY < 96) color = 0xC618;
//                             else if (gridX < 32 && gridY >= 96) color = 0x4208;
//                             else if (gridX >= 32 && gridX < 64 && gridY >= 96) color = 0x000F;
//                             else if (gridX >= 64 && gridX < 96 && gridY >= 96) color = 0x0410;
//                             else color = 0x7BE0;
//                         }
//                         break;
                        
//                     case 4:
//                         {
//                             int val = (x * x / 200 + y * y / 200 + offset) % 128;
//                             if (val < 32) color = ST77XX_BLUE;
//                             else if (val < 64) color = ST77XX_CYAN;
//                             else if (val < 96) color = ST77XX_MAGENTA;
//                             else color = ST77XX_RED;
//                         }
//                         break;
                        
//                     case 5:
//                         {
//                             int blockSize = 20;
//                             bool isBlock = ((x / blockSize) + (y / blockSize) + (offset / 10)) % 2 == 0;
//                             color = isBlock ? ST77XX_CYAN : ST77XX_MAGENTA;
//                         }
//                         break;
                        
//                     case 6:
//                         {
//                             int heat = ((height - y) * 2 + (x + offset) % 40) % 100;
//                             if (heat > 70) color = ST77XX_YELLOW;
//                             else if (heat > 40) color = ST77XX_ORANGE;
//                             else if (heat > 20) color = ST77XX_RED;
//                             else color = ST77XX_BLACK;
//                         }
//                         break;
                        
//                     case 7:
//                         {
//                             int starPos = (x * 17 + y * 13 + offset) % 500;
//                             color = (starPos < 3) ? ST77XX_WHITE : ST77XX_BLACK;
//                         }
//                         break;
                        
//                     default:
//                         color = ((x + y + offset) % 60 < 30) ? ST77XX_CYAN : ST77XX_BLUE;
//                         break;
//                 }
                
//                 line[x] = color;
//             }
//             tft.setAddrWindow(0, y, width, 1);
//             tft.writePixels(line, width);
//         }
        
//         if (mode == 3 || mode == 6) {
//             offset += 3;
//         } else if (mode == 7) {
//             offset += 5;
//         } else {
//             offset += 2;
//         }
        
//         if (offset > 500) offset = 0;
        
//         tft.endWrite();
        
//         tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
//         tft.setTextSize(1);
//         tft.setCursor(5, 230);
//         tft.print("Press ENTER to exit...");
        
//         if (Serial.available()) {
//             char c = Serial.read();
//             if (c == '\n') break;
//         }
        
//         tft.startWrite();
//         vTaskDelay(50 / portTICK_PERIOD_MS);
//     }
    
//     tft.endWrite();
//     applyTheme();
//     screenLocked = false;
//     clearScreen();
// }
void screensaver(int mode) {
    screenLocked = true;
    const int width = 320;
    const int height = 230;
    
    tft.fillScreen(ST77XX_BLACK);
    Serial.println("Press ENTER to exit...");
    
    static uint16_t line[320];
    
    int offset = 0;
    const int targetFPS = 20;  
    const int frameDelay = 1000 / targetFPS;
    
    while (true) {
        unsigned long frameStart = millis();
        
        tft.startWrite();
        
        
        for (int y = 0; y < height; y++) {
            for (int x = 0; x < width; x++) {
                uint16_t color = ST77XX_BLACK;
                
                switch(mode) {
                    case 1: 
                        {
                            float wave = sin((x + offset) * 0.05) * sin((y + offset * 0.5) * 0.05);
                            if (wave > 0) {
                                int intensity = (int)(wave * 255);
                                color = tft.color565(0, intensity, intensity);
                            } else {
                                int intensity = (int)(-wave * 255);
                                color = tft.color565(0, 0, intensity);
                            }
                        }
                        break;
                        
                    case 2: 
                        {
                            float v = tanh(sin(x * 0.04 - offset * 0.01) - cos(y * 0.09) );
                            int i = (int)((v + 1.0) * 127.5);
                            i = constrain(i, 0, 255);
                            int shift = (offset+i / 10) % 256;
                            uint8_t r = sin8(i + offset);      
                            uint8_t g = sin8(i + offset * 2); 
                            uint8_t b = (uint8_t)((sin((i + 128) * M_PI / 128.0) + 1.0) * 127.5);
    
                              color = tft.color565(r, g, b);
                        }
                        break;
                        
                    case 3: 
                        {
                            int dx = x - width / 2;
                            int dy = y - height / 2;
                            float angle = atan2(dy, dx);
                            float dist = sqrt(dx * dx + dy * dy);
                            
                            float spiral = sin(dist * 0.1 - angle * 2 + offset * 0.05) * 0.5 + 0.5;
                            int intensity = (int)(spiral * 255);
                            
                            color = tft.color565(intensity, intensity / 2, 255 - intensity);
                        }
                        break;
                        
                    case 4: 
                        {
                            int col = x / 10;
                            int matrixOffset = (offset + col * 17) % height;
                            
                            if (y > matrixOffset - 20 && y < matrixOffset) {
                                int brightness = (20 - (matrixOffset - y)) * 12;
                                color = tft.color565(0, brightness, 0);
                            } else if (y == matrixOffset) {
                                color = ST77XX_WHITE;
                            }
                        }
                        break;
                        
                    case 5: 
                        {
                            int heat = ((height - y) * 256 / height) + random(-20, 20);
                            heat = constrain(heat, 0, 255);
                            
                            if (heat < 85) {
                                color = tft.color565(heat * 3, 0, 0);
                            } else if (heat < 170) {
                                color = tft.color565(255, (heat - 85) * 3, 0);
                            } else {
                                color = tft.color565(255, 255, (heat - 170) * 3);
                            }
                        }
                        break;
                        
                    case 6: 
                        {
                            int starSeed = (x * 17 + y * 13) % 1000;
                            if (starSeed < 5) {
                                int twinkle = (offset + starSeed * 7) % 30;
                                int brightness = twinkle < 15 ? twinkle * 17 : (30 - twinkle) * 17;
                                color = tft.color565(brightness, brightness, brightness);
                            }
                        }
                        break;
                        
                    case 7: 
                        {
                            int dx = x - width / 2;
                            int dy = y - height / 2;
                            float dist = sqrt(dx * dx + dy * dy);
                            float angle = atan2(dy, dx);
                            
                            if (dist > 1) {
                                int u = (int)(32.0 / dist + offset);
                                int v = (int)(angle * 10 + offset * 0.5);
                                
                                if ((u + v) % 20 < 10) {
                                    int intensity = 255 - (int)(dist * 2);
                                    intensity = constrain(intensity, 0, 255);
                                    color = tft.color565(0, intensity, intensity);
                                }
                            }
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
            
            
            if (y % 10 == 0) {
                tft.endWrite();
                vTaskDelay(1 / portTICK_PERIOD_MS);  
                tft.startWrite();
            }
        }
        
        tft.endWrite();
        
        
        if (mode == 2 || mode == 3) {
            offset += 6;
        } else if (mode == 7) {
            offset += 3;
        } else {
            offset += 2;
        }
        
        if (offset > 10000) offset = 0;
        
        
        tft.setTextColor(ST77XX_WHITE, ST77XX_BLACK);
        tft.setTextSize(1);
        tft.setCursor(5, 230);
        tft.print("Press ENTER to exit...");
        
        
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') break;
        }
        
        
        unsigned long frameTime = millis() - frameStart;
        if (frameTime < frameDelay) {
            vTaskDelay((frameDelay - frameTime) / portTICK_PERIOD_MS);
        } else {
            vTaskDelay(1 / portTICK_PERIOD_MS);  
        }
    }
    
    applyTheme();
    screenLocked = false;
    clearScreen();
    tft.print("> ");
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