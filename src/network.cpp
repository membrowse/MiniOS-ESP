#include "network.h"
#include "display.h"
#include "config.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ESP32Ping.h>

void connectWiFi() {
    if (WiFi.status() == WL_CONNECTED) {
        printLine("Already connected.");
        return;
    }
    printLine("Connecting to WiFi...");
    WiFi.begin(WIFI_SSID, WIFI_PASS);
    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 20) {
        delay(500);
        Serial.print(".");
        attempts++;
    }
    if (WiFi.status() == WL_CONNECTED) {
        printLine("");
        printLine("Connected!");
        printLine("IP: " + WiFi.localIP().toString());
        syncTime();
    } else {
        printLine("");
        printLine("Failed to connect.");
    }
}

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
        delay(500);
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

void curlURL(String url) {
    if (WiFi.status() != WL_CONNECTED) {
        printLine("curl: not connected to WiFi");
        printLine("Run 'wifi' first");
        return;
    }
    
    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);
    
    unsigned long startTime = millis();
    int code = http.GET();
    unsigned long duration = millis() - startTime;
    
    if (code > 0) {
        printLine("HTTP/" + String(http.header("Server").isEmpty() ? "1.1" : "1.1") + " " + String(code));
        
        int headerCount = http.headers();
        if (headerCount > 0) {
            printLine("--- Headers ---");
            for(int i = 0; i < headerCount; i++) {
                printLine(http.headerName(i) + ": " + http.header(i));
            }
        }
        
        printLine("--- Info ---");
        printLine("Content-Length: " + String(http.getSize()));
        printLine("Time: " + String(duration) + "ms");
        
        if (code == HTTP_CODE_OK || code == HTTP_CODE_MOVED_PERMANENTLY || code == HTTP_CODE_FOUND) {
            String payload = http.getString();
            int contentLength = payload.length();
            
            printLine("--- Body (" + String(contentLength) + " bytes) ---");
            
            if (contentLength > 800) {
                printLine(payload.substring(0, 800));
                printLine("");
                printLine("... (" + String(contentLength - 800) + " more bytes)");
                printLine("Response truncated");
            } else if (contentLength > 0) {
                printLine(payload);
            } else {
                printLine("(empty response)");
            }
        } else {
            printLine("--- Error ---");
            printLine("HTTP " + String(code));
        }
    } else {
        printLine("curl: (" + String(code) + ") " + http.errorToString(code));
        printLine("Failed to connect to " + url);
    }
    
    http.end();
}

void scanWiFi() {
    printLine("Scanning WiFi...");
    int n = WiFi.scanNetworks();
    if (n <= 0) {
        printLine("No networks found.");
        return;
    }
    for (int i = 0; i < n; i++) {
        String lock = (WiFi.encryptionType(i) == WIFI_AUTH_OPEN) ? "Open" : "Secured";
        printLine(String(i) + ": " + WiFi.SSID(i) + " (" + String(WiFi.RSSI(i)) + " dBm) " + lock);
    }
}

void pingHost(String host) {
    printLine("Pinging " + host + "...");
    bool ok = Ping.ping(host.c_str(), 3); 
    if (ok) {
        printLine("Ping OK, avg: " + String(Ping.averageTime()) + " ms");
    } else {
        printLine("Ping failed");
    }
}