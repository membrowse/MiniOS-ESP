#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft;
extern bool screenLocked;
extern int16_t currentCursorY;

#define MAX_Y 230

void initDisplay();
void applyTheme();
void clearScreen();
void printLine(String s);
void showLogo();
void screensaver(int mode);
#endif