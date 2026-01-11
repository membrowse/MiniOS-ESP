#ifndef CONFIG_H
#define CONFIG_H

#define TFT_CS   5
#define TFT_DC   2  // Display Pins for ST7789 review README.me for full wiring setup
#define TFT_RST  4


extern const char* WIFI_SSID;
extern const char* WIFI_PASS;
extern const char* NTP_SERVER;
extern const long GMT_OFFSET;
extern const int DAYLIGHT_OFFSET;
extern const char* OS_VERSION;

#endif
