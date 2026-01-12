# MiniOS-ESP

A lightweight command-line operating system for ESP32 with ST7789 display support.

## Features

- **File System**: Read, write, and delete files using SPIFFS  
- **WiFi Connectivity**: Connect to WiFi networks  
- **Time Synchronization**: NTP time sync  
- **HTTP Client**: Fetch web content using curl-like commands  
- **Calculator**: Simple arithmetic calculator  
- **Themes**: Multiple color themes for the display  
- **Serial Command Interface**: Control everything via serial commands  

## Hardware Requirements

- ESP32 development board  
- ST7789 TFT Display (240x320)  
- Wiring:  
  - TFT_CS → GPIO 5  
  - TFT_DC → GPIO 2  
  - TFT_RST → GPIO 4  
  - TFT_SCK → GPIO 18  
  - TFT_SDI → GPIO 23  
  - TFT_BLK → 3.3V or any pin set HIGH  

## Setup

1. **Clone the repository**  
   ```bash
   git clone https://github.com/VuqarAhadli/MiniOS-ESP.git
   cd MiniOS-ESP
   ```

2. **Configure WiFi credentials**  
   Edit `src/config.h` and update your WiFi SSID and password:
   ```cpp
   const char* WIFI_SSID = "your_wifi_ssid";
   const char* WIFI_PASS = "your_wifi_password";
   ```

3. **Upload to ESP32**  
   - Using PlatformIO: `pio run --target upload`  
   - Using Arduino IDE: Open `src/main.cpp` and upload  

4. **Open Serial Monitor**  
   - Set baud rate to 115200  
   - Start typing commands!  

## Available Commands

# MiniOS-ESP Commands

# MiniOS Command Reference

| Command | Description | Example |
|---------|-------------|---------|
| `help` | Show all commands | `help` |
| `os` | Display MiniOS logo | `os` |
| `version` | Show version info | `version` |
| `fetch` | Show system information | `fetch` |
| **File System** | | |
| `write <file> <text>` | Write text to a file | `write notes.txt Hello` |
| `append <file> <text>` | Append text to a file | `append notes.txt World` |
| `read <file>` | Read file contents | `read notes.txt` |
| `delete <file>` | Delete a file | `delete notes.txt` |
| `ls` | List all files | `ls` |
| `mv <old> <new>` | Rename a file | `mv old.txt new.txt` |
| `cp <src> <dst>` | Copy a file | `cp file1.txt file2.txt` |
| **Network** | | |
| `wifi` | Connect to WiFi / show WiFi info | `wifi` |
| `scanwifi` | Scan nearby WiFi networks | `scanwifi` |
| `curl <url>` | Fetch URL content | `curl https://example.com` |
| `ping <host>` | Ping host 3 times | `ping 8.8.8.8` |
| **Time** | | |
| `time` | Show current time | `time` |
| `synctime` | Sync time with NTP server | `synctime` |
| **System** | | |
| `mem` | Show free heap memory | `mem` |
| `uptime` | Show time since boot | `uptime` |
| `reboot` | Reboot device | `reboot` |
| `calc <expression>` | Evaluate math expression | `calc 15 + 27 * 2` |
| **Display** | | |
| `themes` | List available themes | `themes` |
| `theme <name\|number>` | Set display theme | `theme matrix` or `theme 0` |
| `pug` | Display pug image | `pug` |
| `screensaver <number>` | Display screensaver animation | `screensaver 3` |



## Themes

- `matrix` (0) — Green on black (**Default**)  
- `cyan` (1) — Cyan on black  
- `classic` (2) — White on black  
- `blue` (3) — Yellow on blue  
- `light` (4) — Black on white  
- `dark-orange` (5) — Orange on dark  
- `purple` (6) — Magenta on dark  
- `red night` (7) — Red on dark  

## Screensaver Modes
1. Diagonal Waves
2. Rainbow Wave
3. Color Grid
4. Plasma
5. Checkerboard
6. Fire
7. Starfield


## Project Structure

```
MiniOS-ESP/
├── src/                 # Source files
│   ├── main.cpp         # Main program loop
│   ├── commands.cpp     # Command implementations
│   ├── display.cpp      # TFT display functions
│   ├── filesystem.cpp   # SPIFFS file operations
│   ├── network.cpp      # WiFi and HTTP functionality
│   ├── pug.cpp
│   ├── config.cpp       # WiFi credentials and general config
│   └── theme.cpp        # Theme management
├── include/             # Header files
│   ├── commands.h
│   ├── config.h         
│   ├── display.h
│   ├── filesystem.h
│   ├── network.h
│   ├── pug.h
│   └── theme.h
├── lib/
│   └── Adafruit ST7789 Library
├── platformio.ini       # Build/upload configuration
├── README.md            # Project documentation
└── .gitignore           # Ignored files
```

## Dependencies

This project uses the following open-source libraries:  

- [Adafruit GFX Library](https://github.com/adafruit/Adafruit-GFX-Library)  
- [Adafruit ST7789 Library](https://github.com/adafruit/Adafruit-ST7789-Library)  
- ESP32 Arduino Core (built into PlatformIO and Arduino IDE for ESP32)  

Install these via PlatformIO Library Manager or Arduino Library Manager.

## Contributing

Contributions are welcome! Feel free to fork the repo and submit Pull Requests.

## License

MIT License

## Author

**VuqarAhadli**  
GitHub Repository: [https://github.com/VuqarAhadli/MiniOS-ESP](https://github.com/VuqarAhadli/MiniOS-ESP)

## Version

MiniOS-ESP v1.2.0
