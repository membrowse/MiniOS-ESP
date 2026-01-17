# MiniOS v2.0.1 - Patch Notes

## 🎯 Core Improvements

### Command System Overhaul
- **Refactored command parser** with structured argument handling
- **Case-insensitive commands** - now accepts PING, ping, Ping, etc.
- **Command aliases added** for improved usability:
  - `rm` → delete
  - `dir` → ls  
  - `free` → mem
  - `restart` → reboot
  - `neofetch` or `fastfetch` → fetch
  - `wifiscan` → scanwifi
  - `ipconfig` → ifconfig
  - `dns` → nslookup
  - `date` → time
  - `ntpupdate` → synctime
  - `cal` → calendar
  - `sw` → stopwatch
  - `ss` → screensaver
  - `plot` → graph
  - `top` → ps
  - `h` → help

### Network Stack Enhancements
- **Improved curl implementation**
  - Verbose mode with `-v` flag
  - Better HTTP status code handling (429, 504 added)
  - Binary content detection
  - Automatic redirect following
  - Response truncation for large payloads (1500 bytes)
  - Better error messages
- **Enhanced WiFi connection**
  - Ability to scan WI-FI without connection
  - Disconnect function
  - Connection status display with RSSI
- **Improved ping utility**
  - Min/avg/max round-trip time statistics
  - Better timeout handling
  - Packet loss percentage
- **DNS lookup command** added (`nslookup`/`dns`)
- **Network info command** (`ifconfig`/`ipconfig`)

### Display & UI
- **Better screen clearing logic**
  - Clears when near bottom of screen
  - Prevents duplicate prompts
  - Improved cursor positioning

### Help System
- **Enhanced help documentation**
  - Organized by category
  - Command aliases shown inline
  - Better usage examples
  - Consistent formatting

## Bug Fixes
- Fixed base64 command argument parsing
- Fixed screen overflow handling
- Fixed cursor position after clear
- Fixed empty command handling
- Corrected ESP32Ping library usage (was using ESP8266 version)

##  Technical Changes
- Improved error handling across all commands
- Streamlined code structure without comments

##  Updated Commands
All help menus updated to reflect new aliases and improved descriptions.

---
**Repository:** github.com/VuqarAhadli  
**Version:** 2.0.1