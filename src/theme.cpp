#include "theme.h"
#include "display.h"
#include <Adafruit_ST7789.h>

Theme themes[] = {
    {ST77XX_BLACK, ST77XX_GREEN, "matrix"},
    {ST77XX_BLACK, ST77XX_CYAN, "cyan"},
    {ST77XX_BLACK, ST77XX_WHITE, "classic"},
    {ST77XX_BLUE, ST77XX_YELLOW, "blue"},
    {ST77XX_WHITE, ST77XX_BLACK, "light"},
    {0x0014, ST77XX_ORANGE, "dark-orange"},
    {0x2104, ST77XX_MAGENTA, "purple"},
    {ST77XX_BLACK, ST77XX_RED, "red night"}
};

int currentTheme = 0;
int themeCount = sizeof(themes) / sizeof(Theme);

void initThemes() {}

void listThemes() {
    printLine("Available themes:");
    for (int i = 0; i < themeCount; i++) {
        String marker = (i == currentTheme) ? " *" : "";
        printLine(String(i) + ": " + themes[i].name + marker);
    }
}

void setTheme(String tn) {
    int themeNum = tn.toInt();
    
    if (tn == "0" || themeNum > 0) {
        if (themeNum >= 0 && themeNum < themeCount) {
            currentTheme = themeNum;
            applyTheme();
            printLine("Theme set: " + String(themes[currentTheme].name));
            return;
        }
    }
    
    for (int i = 0; i < themeCount; i++) {
        if (tn == themes[i].name) {
            currentTheme = i;
            applyTheme();
            printLine("Theme set: " + String(themes[currentTheme].name));
            return;
        }
    }
    
    printLine("Invalid theme.");
    printLine("Use 'themes' to list.");
}

Theme getCurrentTheme() {
    return themes[currentTheme];
}