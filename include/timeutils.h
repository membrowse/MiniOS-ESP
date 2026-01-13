#ifndef TIMEUTILS_H
#define TIMEUTILS_H


#include <Arduino.h>

void syncTime();
String getTime();
void showCalendar();
void timerCommand(int seconds);
void stopwatchCommand();
void setAlarm(String timeStr);
void checkAlarm();

struct Alarm {
    bool active;
    int hour;
    int minute;
    String message;
};

extern Alarm systemAlarm;

#endif