#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>

void connectWiFi();
void syncTime();
String getTime();
void curlURL(String url);

#endif