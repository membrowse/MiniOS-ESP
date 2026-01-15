#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>

void runCommand(String cmd);
void showVersion();
void showHelp();
void showHelpOS();
void calc(String expression);
void showMem();
void showUptime();
void doReboot();
void showChipInfo();
void showCPUInfo();
void showFlashInfo();
void showWiFiInfo();
void fetch();
void echoCommand(String text);
void addToHistory(String cmd);
void showHistory();

void hexCommand(String numStr);
void binCommand(String numStr);
void base64Command(String operation, String text);
String base64Encode(String input);
String base64Decode(String input);


void processCommand(String args);
void showSystemStats();
void killProcessCmd(int pid);



#endif