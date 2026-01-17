#ifndef NETWORK_H
#define NETWORK_H

#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>


extern String WIFI_SSID;
extern String WIFI_PASS;


enum NetworkStatus {
    NET_DISCONNECTED,
    NET_CONNECTING,
    NET_CONNECTED,
    NET_FAILED
};

extern NetworkStatus networkStatus;

void connectWiFi();
void disconnectWiFi();
void scanWiFi();
void showNetworkInfo();
bool isConnected();
String getLocalIP();
int getSignalStrength();

void curlURL(String url);
void curlURLVerbose(String url);
String httpGet(String url);
int httpPost(String url, String data);

void pingHost(String host);
void dnsLookup(String hostname);

String getStatusText(int httpCode);
String formatBytes(int bytes);
bool isBinaryContent(String contentType);

struct CurlOptions {
    String url;
    String method = "GET";
    String data = "";
    bool verbose = false;
    bool followRedirects = true;
    int timeout = 15000;
    String userAgent = "MiniOS-curl/1.0";

    String headers[5];
    int headerCount = 0;
    
    void addHeader(String header) {
        if (headerCount < 5) {
            headers[headerCount++] = header;
        }
    }
};

void curlWithOptions(CurlOptions opts);

struct CurlParams {
    String url;
    bool verbose;
};

void curlProcess(void* parameter);

#endif