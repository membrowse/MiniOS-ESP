#ifndef DISPLAY_H
#define DISPLAY_H

#include <Arduino.h>
#include <Adafruit_ST7789.h>

extern Adafruit_ST7789 tft;

void initDisplay();
void applyTheme();
void clearScreen();
void printLine(String s);
void showLogo();

#endif