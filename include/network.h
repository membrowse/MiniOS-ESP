#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
extern String WIFI_SSID;
extern String WIFI_PASS;
void connectWiFi();
void curlURL(String url);
void scanWiFi();
void pingHost(String host);

#endif