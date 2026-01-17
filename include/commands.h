#ifndef COMMANDS_H
#define COMMANDS_H

#include <Arduino.h>

void runCommand(String cmd);
void showVersion();
void showHelp();
void showHelpOS();
int precedence(char op);
bool isRightAssociative(char op);
float applyOp(float a, float b, char op);
bool isFunction(const String& expr, int pos, const String& func);
String getFunctionName(const String& expr, int pos);
float applyFunction(const String& func, float value);
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