#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>

void runCommand(String cmd);
void showVersion();
void showHelp();
void calc(String expression);

#endif