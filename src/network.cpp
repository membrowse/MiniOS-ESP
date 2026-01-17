#include "network.h"
#include "display.h"
#include "timeutils.h"
#include <WiFi.h>
#include <HTTPClient.h>
#include <ESP32Ping.h>

String WIFI_SSID = "";
String WIFI_PASS = "";
NetworkStatus networkStatus = NET_DISCONNECTED;
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

void disconnectWiFi() {
    if (WiFi.status() != WL_CONNECTED) {
        printLine("Not connected to WiFi");
        return;
    }
    
    WiFi.disconnect();
    networkStatus = NET_DISCONNECTED;
    printLine("WiFi disconnected");
}

void scanWiFi() {
    printLine("Scanning networks...");
    
    int n = WiFi.scanNetworks();
    
    if (n == 0) {
        printLine("No networks found");
    } else {
        printLine("Found " + String(n) + " networks:");
        printLine("");
        printLine("  SSID                   RSSI  Ch  Enc");
        printLine("  ----------------------------------------");
        
        for (int i = 0; i < n; i++) {
            String ssid = WiFi.SSID(i);
            if (ssid.length() > 20) ssid = ssid.substring(0, 20);
            
            String encryption;
            switch (WiFi.encryptionType(i)) {
                case WIFI_AUTH_OPEN: encryption = "Open"; break;
                case WIFI_AUTH_WEP: encryption = "WEP"; break;
                case WIFI_AUTH_WPA_PSK: encryption = "WPA"; break;
                case WIFI_AUTH_WPA2_PSK: encryption = "WPA2"; break;
                case WIFI_AUTH_WPA_WPA2_PSK: encryption = "WPA/2"; break;
                default: encryption = "WPA2"; break;
            }
            
            char line[60];
            sprintf(line, "%2d %-20s %4d  %2d  %s",
                    i + 1,
                    ssid.c_str(),
                    WiFi.RSSI(i),
                    WiFi.channel(i),
                    encryption.c_str());
            printLine(line);
        }
    }
    
    WiFi.scanDelete();
}

void showNetworkInfo() {
    if (WiFi.status() != WL_CONNECTED) {
        printLine("Not connected to WiFi");
        return;
    }
    
    printLine("IP Address: " + WiFi.localIP().toString());
    printLine("Gateway: " + WiFi.gatewayIP().toString());
    printLine("Subnet: " + WiFi.subnetMask().toString());
    printLine("DNS: " + WiFi.dnsIP().toString());
    printLine("MAC: " + WiFi.macAddress());
    printLine("RSSI: " + String(WiFi.RSSI()) + " dBm");
    printLine("Channel: " + String(WiFi.channel()));
}

bool isConnected() {
    return WiFi.status() == WL_CONNECTED;
}

String getLocalIP() {
    return WiFi.localIP().toString();
}

int getSignalStrength() {
    return WiFi.RSSI();
}


void curlURL(String url) {
    CurlOptions opts;
    opts.url = url;
    opts.verbose = false;
    curlWithOptions(opts);
}

void curlURLVerbose(String url) {
    CurlOptions opts;
    opts.url = url;
    opts.verbose = true;
    curlWithOptions(opts);
}

void curlWithOptions(CurlOptions opts) {
    if (!isConnected()) {
        printLine("curl: not connected to WiFi");
        printLine("Run 'wifi' first");
        return;
    }

    if (!opts.url.startsWith("http://") && !opts.url.startsWith("https://")) {
        printLine("curl: invalid URL (must start with http:// or https://)");
        return;
    }

    HTTPClient http;
    http.begin(opts.url);
    http.setTimeout(opts.timeout);
    
    if (opts.followRedirects) {
        http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    }
    
    http.setUserAgent(opts.userAgent);
    
    for (int i = 0; i < opts.headerCount; i++) {
        int colonPos = opts.headers[i].indexOf(':');
        if (colonPos > 0) {
            String headerName = opts.headers[i].substring(0, colonPos);
            String headerValue = opts.headers[i].substring(colonPos + 1);
            headerValue.trim();
            http.addHeader(headerName, headerValue);
        }
    }
    
    if (opts.verbose) {
        printLine("> " + opts.method + " " + opts.url);
        printLine("> User-Agent: " + opts.userAgent);
    }
    
    unsigned long startTime = millis();
    int code;
    
    if (opts.method == "POST") {
        code = http.POST(opts.data);
    } else if (opts.method == "PUT") {
        code = http.PUT(opts.data);
    } else if (opts.method == "DELETE") {
        code = http.sendRequest("DELETE");
    } else {
        code = http.GET();
    }
    
    unsigned long duration = millis() - startTime;
    
    if (code <= 0) {
        printLine("curl: connection failed");
        printLine("Error: " + http.errorToString(code));
        http.end();
        return;
    }

    if (opts.verbose) {
        printLine("< HTTP/1.1 " + String(code) + " " + getStatusText(code));
        printLine("< Request-Time: " + String(duration) + "ms");
    } else {
        printLine("HTTP " + String(code) + " - " + String(duration) + "ms");
    }
    
    if (opts.verbose) {
        if (http.hasHeader("Content-Type")) {
            printLine("< Content-Type: " + http.header("Content-Type"));
        }
        if (http.hasHeader("Content-Length")) {
            printLine("< Content-Length: " + http.header("Content-Length"));
        }
        if (http.hasHeader("Server")) {
            printLine("< Server: " + http.header("Server"));
        }
        printLine("");
    }

    
    if (code == HTTP_CODE_OK) {
        int contentLength = http.getSize();
        String contentType = http.header("Content-Type");
        
        if (isBinaryContent(contentType)) {
            printLine("Binary content (" + contentType + ")");
            printLine("Size: " + formatBytes(contentLength));
            printLine("Cannot display binary data");
        } else {
           
            String payload = http.getString();
            int len = payload.length();
            
            if (len > 1500) {
                printLine(payload.substring(0, 1500));
                printLine("");
                printLine("... (+" + String(len - 1500) + " bytes)");
                printLine("Response truncated at 1500 bytes");
            } else if (len > 0) {
                printLine(payload);
            } else {
                printLine("(empty response)");
            }
        }
    } else if (code >= 300 && code < 400) {
        if (http.hasHeader("Location")) {
            printLine("Redirect to: " + http.header("Location"));
        }
    } else if (code >= 400) {
        
        printLine("Error " + String(code) + ": " + getStatusText(code));
        String payload = http.getString();
        if (payload.length() > 300) {
            printLine(payload.substring(0, 300) + "...");
        } else if (payload.length() > 0) {
            printLine(payload);
        }
    }

    http.end();
}

String httpGet(String url) {
    if (!isConnected()) return "";
    
    HTTPClient http;
    http.begin(url);
    http.setTimeout(10000);
    
    int code = http.GET();
    String result = "";
    
    if (code == HTTP_CODE_OK) {
        result = http.getString();
    }
    
    http.end();
    return result;
}

int httpPost(String url, String data) {
    if (!isConnected()) return -1;
    
    HTTPClient http;
    http.begin(url);
    http.addHeader("Content-Type", "application/json");
    http.setTimeout(10000);
    
    int code = http.POST(data);
    http.end();
    
    return code;
}


void pingHost(String host) {
    if (!isConnected()) {
        printLine("ping: not connected to WiFi");
        return;
    }
    
    printLine("PING " + host);
    
    IPAddress ip;
    if (!WiFi.hostByName(host.c_str(), ip)) {
        printLine("ping: cannot resolve " + host);
        return;
    }
    
    printLine("Pinging " + ip.toString() + "...");
    
    int sent = 0;
    int received = 0;
    float totalTime = 0;
    float minTime = 999999;
    float maxTime = 0;
    
    for (int i = 0; i < 4; i++) {
        sent++;
        
        bool success = Ping.ping(ip, 1);
        
        if (success) {
            received++;
            float time = Ping.averageTime();
            totalTime += time;
            
            if (time < minTime) minTime = time;
            if (time > maxTime) maxTime = time;
            
            printLine(String(i + 1) + ": Reply from " + ip.toString() + 
                     " time=" + String(time, 1) + "ms");
        } else {
            printLine(String(i + 1) + ": Request timeout");
        }
        
        if (i < 3) {
            vTaskDelay(1000 / portTICK_PERIOD_MS);
        }
    }
    
    printLine("");
    printLine("--- " + host + " ping statistics ---");
    printLine(String(sent) + " packets sent, " + String(received) + " received, " +
             String(((sent - received) * 100) / sent) + "% packet loss");
    
    if (received > 0) {
        float avgTime = totalTime / received;
        printLine("Round-trip min/avg/max = " + 
                 String(minTime, 1) + "/" + 
                 String(avgTime, 1) + "/" + 
                 String(maxTime, 1) + " ms");
    }
}

void dnsLookup(String hostname) {
    if (!isConnected()) {
        printLine("dns: not connected to WiFi");
        return;
    }
    
    printLine("Looking up: " + hostname);
    
    IPAddress ip;
    if (WiFi.hostByName(hostname.c_str(), ip)) {
        printLine("IP Address: " + ip.toString());
    } else {
        printLine("DNS lookup failed");
    }
}


String getStatusText(int code) {
    switch(code) {
        case 200: return "OK";
        case 201: return "Created";
        case 204: return "No Content";
        case 301: return "Moved Permanently";
        case 302: return "Found";
        case 304: return "Not Modified";
        case 400: return "Bad Request";
        case 401: return "Unauthorized";
        case 403: return "Forbidden";
        case 404: return "Not Found";
        case 429: return "Too Many Requests";
        case 500: return "Internal Server Error";
        case 502: return "Bad Gateway";
        case 503: return "Service Unavailable";
        case 504: return "Gateway Timeout";
        default: return "";
    }
}

String formatBytes(int bytes) {
    if (bytes < 1024) {
        return String(bytes) + " B";
    } else if (bytes < 1024 * 1024) {
        return String(bytes / 1024.0, 2) + " KB";
    } else {
        return String(bytes / 1024.0 / 1024.0, 2) + " MB";
    }
}

bool isBinaryContent(String contentType) {
    contentType.toLowerCase();
    return contentType.indexOf("image/") >= 0 ||
           contentType.indexOf("video/") >= 0 ||
           contentType.indexOf("audio/") >= 0 ||
           contentType.indexOf("application/pdf") >= 0 ||
           contentType.indexOf("application/zip") >= 0 ||
           contentType.indexOf("application/octet-stream") >= 0;
}



void curlProcess(void* parameter) {
    CurlParams* params = (CurlParams*)parameter;
    
    extern bool inputLocked;
    inputLocked = true;
    
    if (params->verbose) {
        curlURLVerbose(params->url);
    } else {
        curlURL(params->url);
    }
    
    delete params;
    inputLocked = false;
    
    vTaskDelete(NULL);
}