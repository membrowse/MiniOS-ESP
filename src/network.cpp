#include "network.h"
#include "display.h"
#include "config.h"
#include <timeutils.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <time.h>
#include <ESP32Ping.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

String WIFI_SSID = "";
String WIFI_PASS = "";

extern bool inputLocked;

void connectWiFi() {
    inputLocked = true;
    vTaskDelay(100 / portTICK_PERIOD_MS);  

    if (WiFi.status() == WL_CONNECTED) {
        printLine("Already connected!");
        printLine("SSID: " + WiFi.SSID());
        printLine("IP: " + WiFi.localIP().toString());
        printLine("RSSI: " + String(WiFi.RSSI()) + " dBm");
        inputLocked = false;
        return;
    }

    while (Serial.available() > 0) Serial.read();

    printLine("Enter SSID: ");
    WIFI_SSID = "";
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                if (WIFI_SSID.length() > 0) {
                    Serial.println();
                    break;
                }
            } else if (c == '\b' || c == 127) {
                if (WIFI_SSID.length() > 0) {
                    WIFI_SSID.remove(WIFI_SSID.length() - 1);
                    Serial.write('\b'); Serial.write(' '); Serial.write('\b');
                }
            } else if (c >= 32 && c <= 126) {
                WIFI_SSID += c;
                Serial.write(c);
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    WIFI_SSID.trim();

    while (Serial.available() > 0) Serial.read();

    printLine("Enter Password: ");
    WIFI_PASS = "";
    while (true) {
        if (Serial.available()) {
            char c = Serial.read();
            if (c == '\n' || c == '\r') {
                if (WIFI_PASS.length() > 0) {
                    Serial.println();
                    break;
                }
            } else if (c == '\b' || c == 127) {
                if (WIFI_PASS.length() > 0) {
                    WIFI_PASS.remove(WIFI_PASS.length() - 1);
                    Serial.write('\b'); Serial.write(' '); Serial.write('\b');
                }
            } else if (c >= 32 && c <= 126) {
                WIFI_PASS += c;
                Serial.write('*');
            }
        }
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
    WIFI_PASS.trim();

    inputLocked = false;

    printLine("Connecting to: " + WIFI_SSID);
    WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());

    int attempts = 0;
    while (WiFi.status() != WL_CONNECTED && attempts < 10) {
        vTaskDelay(500 / portTICK_PERIOD_MS);
        Serial.print(".");
        tft.print(".");
        attempts++;
    }

    if (WiFi.status() == WL_CONNECTED) {
        printLine("");
        printLine("Connected!");
        printLine("SSID: " + WiFi.SSID());
        printLine("IP: " + WiFi.localIP().toString());
        printLine("RSSI: " + String(WiFi.RSSI()) + " dBm");
        syncTime();
    } else {
        printLine("");
        printLine("Failed to connect.");
    }
}

// void connectWiFi() {
//     inputLocked = true;
//     vTaskDelay(100 / portTICK_PERIOD_MS);  
    
//     while (Serial.available() > 0) {
//         Serial.read();
//     }
    
//     printLine("Enter SSID: ");
    
//     WIFI_SSID = "";
//     while (true) {
//         if (Serial.available()) {
//             char c = Serial.read();
//             if (c == '\n' || c == '\r') {
//                 if (WIFI_SSID.length() > 0) {
//                     Serial.println();
//                     break;
//                 }
//             } else if (c == '\b' || c == 127) {
//                 if (WIFI_SSID.length() > 0) {
//                     WIFI_SSID.remove(WIFI_SSID.length() - 1);
//                     Serial.write('\b');
//                     Serial.write(' ');
//                     Serial.write('\b');
//                 }
//             } else if (c >= 32 && c <= 126) {  
//                 WIFI_SSID += c;
//                 Serial.write(c);
//             }
//         }
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
//     WIFI_SSID.trim();
    

//     while (Serial.available() > 0) {
//         Serial.read();
//     }
    
//     printLine("Enter Password: ");
    
//     WIFI_PASS = "";
//     while (true) {
//         if (Serial.available()) {
//             char c = Serial.read();
//             if (c == '\n' || c == '\r') {
//                 if (WIFI_PASS.length() > 0) {
//                     Serial.println();
//                     break;
//                 }
//             } else if (c == '\b' || c == 127) {
//                 if (WIFI_PASS.length() > 0) {
//                     WIFI_PASS.remove(WIFI_PASS.length() - 1);
//                     Serial.write('\b');
//                     Serial.write(' ');
//                     Serial.write('\b');
//                 }
//             } else if (c >= 32 && c <= 126) {  
//                 WIFI_PASS += c;
//                 Serial.write('*');  
//             }
//         }
//         vTaskDelay(10 / portTICK_PERIOD_MS);
//     }
//     WIFI_PASS.trim();
    
//     inputLocked = false;  
    
//     if (WiFi.status() == WL_CONNECTED) {
//         printLine("Already connected.");
//         return;
//     }
    
//     printLine("Connecting to: " + WIFI_SSID);
//     WiFi.begin(WIFI_SSID.c_str(), WIFI_PASS.c_str());
    
//     int attempts = 0;
//     while (WiFi.status() != WL_CONNECTED && attempts < 20) {
//         vTaskDelay(500 / portTICK_PERIOD_MS);
//         Serial.print(".");
//         attempts++;
//     }
    
//     if (WiFi.status() == WL_CONNECTED) {
//         printLine("");
//         printLine("Connected!");
//         printLine("IP: " + WiFi.localIP().toString());
//         syncTime();
//     } else {
//         printLine("");
//         printLine("Failed to connect.");
//     }
// }

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