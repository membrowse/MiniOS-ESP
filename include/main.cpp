#include <Arduino.h>
#include "config.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "commands.h"

String input = "";

void setup() {
    Serial.begin(115200);
    
    initDisplay();
    initThemes();
    
    if (!initFilesystem()) {
        while (1);
    }
    
    printLine("MiniOS booted.");
    printLine("Type commands in Serial.");
    printLine("> ");
}

void loop() {
    while (Serial.available()) {
        char c = Serial.read();
        if (c == '\n') {
            clearScreen();
            printLine("> " + input);
            runCommand(input);
            input = "";
            printLine("\n> ");
        } else {
            input += c;
        }
    }
}