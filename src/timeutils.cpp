#include "commands.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "config.h"
#include "pug.h"
#include "timeutils.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

Alarm systemAlarm = {false, 0, 0, ""};

void syncTime() {
    if (WiFi.status() != WL_CONNECTED) {
        printLine("WiFi not connected.");
        printLine("Use 'wifi' first.");
        return;
    }
    
    printLine("Syncing time...");
    configTime(GMT_OFFSET, DAYLIGHT_OFFSET, NTP_SERVER);
    
    int attempts = 0;
    while (time(nullptr) < 100000 && attempts < 20) {
        vTaskDelay(500 / portTICK_PERIOD_MS);  
        Serial.print(".");
        attempts++;
    }
    
    if (time(nullptr) > 100000) {
        printLine("");
        printLine("Time synced!");
        printLine(getTime());
    } else {
        printLine("");
        printLine("Time sync failed.");
    }
    
    
    vTaskDelay(100 / portTICK_PERIOD_MS);
    while (Serial.available() > 0) {
        Serial.read();
    }
}

String getTime() {
    time_t now = time(nullptr);
    
    if (now < 100000) {
        return "Time not synced";
    }
    
    struct tm* t = localtime(&now);
    char buf[40];
    sprintf(buf, "%04d-%02d-%02d  %02d:%02d:%02d",
            t->tm_year + 1900,
            t->tm_mon + 1,
            t->tm_mday,
            t->tm_hour,
            t->tm_min,
            t->tm_sec);
    return String(buf);
}

void showCalendar() {
    time_t now = time(nullptr);
    if (now < 100000) {
        printLine("Time not synced. Use 'synctime' first.");
        return;
    }
    
    struct tm* t = localtime(&now);
    int year = t->tm_year + 1900;
    int month = t->tm_mon + 1;
    int day = t->tm_mday;
    
    const char* monthNames[] = {
        "January", "February", "March", "April", "May", "June",
        "July", "August", "September", "October", "November", "December"
    };
    
    printLine(String(monthNames[month - 1]) + " " + String(year));
    printLine("Mo Tu We Th Fr Sa Su");
    
    struct tm firstDay = *t;
    firstDay.tm_mday = 1;
    mktime(&firstDay);
    int startDay = (firstDay.tm_wday + 6) % 7;
    
    int daysInMonth[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if ((year % 4 == 0 && year % 100 != 0) || (year % 400 == 0)) {
        daysInMonth[1] = 29;
    }
    
    String line = "";
    for (int i = 0; i < startDay; i++) {
        line += "   ";
    }
    
    for (int d = 1; d <= daysInMonth[month - 1]; d++) {
        if (d == day) {
            line += "[" + String(d) + "]";
            if (d < 10) line += " ";
        } else {
            if (d < 10) line += " ";
            line += String(d) + " ";
        }
        
        if ((startDay + d) % 7 == 0) {
            printLine(line);
            line = "";
        }
    }
    if (line.length() > 0) {
        printLine(line);
    }
}

void timerCommand(int seconds) {
    if (seconds <= 0) {
        printLine("Invalid timer duration.");
        return;
    }
    
    printLine("Timer started for " + String(seconds) + " seconds.");
    printLine("Press ENTER to cancel...");
    
    unsigned long startTime = millis();
    unsigned long endTime = startTime + (seconds * 1000);
    
    while (millis() < endTime) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                printLine("Timer cancelled.");
                return;
            }
        }
        
        unsigned long remaining = (endTime - millis()) / 1000;
        if (remaining != ((endTime - millis() - 1000) / 1000)) {
            printLine(String(remaining) + " seconds remaining...");
        }
        
        vTaskDelay(100 / portTICK_PERIOD_MS);  
    }
    
    printLine("Timer finished!");
    for (int i = 0; i < 3; i++) {
        printLine("BEEP!");
        vTaskDelay(200 / portTICK_PERIOD_MS);  
    }
}

void stopwatchCommand() {
    printLine("Stopwatch started.");
    printLine("Press ENTER to stop...");
    Theme currenttheme = getCurrentTheme();
    unsigned long startTime = millis();
    unsigned long lastUpdate = 0;
    String lastTime = "";
    int16_t timeX = tft.getCursorX();
    int16_t timeY = tft.getCursorY();
    
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n') {
                unsigned long elapsed = millis() - startTime;
                unsigned long s = elapsed / 1000;
                unsigned long ms = elapsed % 1000;
                unsigned long h = s / 3600;
                unsigned long m = (s % 3600) / 60;
                unsigned long sec = s % 60;
                printLine("");
                printLine("Stopped at: " + String(h) + "h " + String(m) + "m " + 
                          String(sec) + "s " + String(ms) + "ms");
                        
                return;
            }
        }
        
        unsigned long current = millis();
        if (current - lastUpdate >= 1000) {
            unsigned long elapsed = current - startTime;
            unsigned long s = elapsed / 1000;
            unsigned long h = s / 3600;
            unsigned long m = (s % 3600) / 60;
            unsigned long sec = s % 60;
            
            String currentTime = String(h) + "h " + String(m) + "m " + String(sec) + "s";
            
            if (currentTime != lastTime) {
                tft.setCursor(timeX+5, timeY);
                tft.setTextColor(currenttheme.bg);
                tft.print(lastTime);

                tft.setCursor(timeX+5, timeY);
                tft.setTextColor(currenttheme.fg);
                tft.print(currentTime);
                
                lastTime = currentTime;
            }
            lastUpdate = current;
        }
    }
}

void setAlarm(String timeStr) {
    int colonPos = timeStr.indexOf(':');
    if (colonPos == -1) {
        printLine("Usage: alarm HH:MM [message]");
        printLine("Example: alarm 14:30 Meeting time");
        return;
    }
    
    String hourStr = timeStr.substring(0, colonPos);
    String rest = timeStr.substring(colonPos + 1);
    
    int spacePos = rest.indexOf(' ');
    String minuteStr;
    String message = "";
    
    if (spacePos != -1) {
        minuteStr = rest.substring(0, spacePos);
        message = rest.substring(spacePos + 1);
    } else {
        minuteStr = rest;
    }
    
    int hour = hourStr.toInt();
    int minute = minuteStr.toInt();
    
    if (hour < 0 || hour > 23 || minute < 0 || minute > 59) {
        printLine("Invalid time format.");
        return;
    }
    
    systemAlarm.active = true;
    systemAlarm.hour = hour;
    systemAlarm.minute = minute;
    systemAlarm.message = message;
    
    printLine("Alarm set for " + String(hour) + ":" + 
             (minute < 10 ? "0" : "") + String(minute));
    if (message.length() > 0) {
        printLine("Message: " + message);
    }
}

void checkAlarm() {
    if (!systemAlarm.active) return;
    
    time_t now = time(nullptr);
    if (now < 100000) return;
    
    struct tm* t = localtime(&now);
    
    if (t->tm_hour == systemAlarm.hour && t->tm_min == systemAlarm.minute) {
        printLine("");
        printLine("*** ALARM! ***");
        if (systemAlarm.message.length() > 0) {
            printLine(systemAlarm.message);
        }
        printLine("*** ALARM! ***");
        printLine("");
        systemAlarm.active = false;
    }
}