#ifndef THEME_H
#define THEME_H

#include <Arduino.h>

struct Theme {
    uint16_t bg;
    uint16_t fg;
    const char* name;
};

extern Theme themes[];
extern int currentTheme;
extern int themeCount;

void initThemes();
void listThemes();
void setTheme(String themeName);
Theme getCurrentTheme();

#endif