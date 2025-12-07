#ifndef CONFIG_H
#define CONFIG_H


#define TFT_CS   5
#define TFT_DC   2 /*Display Pins for ST7789*/
#define TFT_RST  4


const char* WIFI_SSID = "Fill it";
const char* WIFI_PASS = "Fill it";


const char* NTP_SERVER = "pool.ntp.org"; /*For time*/
const long GMT_OFFSET = 4 * 3600;
const int DAYLIGHT_OFFSET = 0;


const char* OS_VERSION = "MiniOS-ESP v1.0";

#endif