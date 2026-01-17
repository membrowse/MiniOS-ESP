#include "commands.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "config.h"
#include "pug.h"
#include "timeutils.h"
#include "kernel.h"
#include "grapher.h"
#include <esp_system.h>
#include <WiFi.h>
#include <math.h>

#define PI 3.14159265358979323846
#define E  2.71828182845904523536
#define HISTORY_SIZE 10  
const char base64_chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";
String commandHistory[HISTORY_SIZE];
int historyIndex = 0;
int historyCount = 0;





void showVersion() {
    printLine("MiniOS " + String(OS_VERSION));
    printLine("Repository: github.com/VuqarAhadli");
}


void addToHistory(String cmd) {
    if (cmd.length() == 0) return;
    if (historyCount > 0 && commandHistory[(historyIndex - 1 + HISTORY_SIZE) % HISTORY_SIZE] == cmd) return;
    commandHistory[historyIndex] = cmd;
    historyIndex = (historyIndex + 1) % HISTORY_SIZE;
    if (historyCount < HISTORY_SIZE) historyCount++;
}

void showHistory() {
    if (historyCount == 0) {
        printLine("No command history.");
        return;
    }
    printLine("Command history:");
    int start = (historyIndex - historyCount + HISTORY_SIZE) % HISTORY_SIZE;
    for (int i = 0; i < historyCount; i++) {
        int idx = (start + i) % HISTORY_SIZE;
        printLine(String(i + 1) + ": " + commandHistory[idx]);
    }
}



void hexCommand(String numStr) {
    long num = numStr.toInt();
    char hexStr[20];
    sprintf(hexStr, "0x%lX", num);
    printLine("Decimal: " + String(num));
    printLine("Hexadecimal: " + String(hexStr));
}

void binCommand(String numStr) {
    long num = numStr.toInt();
    String binStr = "";
    
    if (num == 0) {
        binStr = "0";
    } else {
        long temp = num;
        while (temp > 0) {
            binStr = String(temp % 2) + binStr;
            temp /= 2;
        }
    }
    
    printLine("Decimal: " + String(num));
    printLine("Binary: 0b" + binStr);
}


String base64Encode(String input) {
    String output = "";
    int val = 0;
    int valb = -6;
    
    for (unsigned int i = 0; i < input.length(); i++) {
        unsigned char c = input[i];
        val = (val << 8) + c;
        valb += 8;
        while (valb >= 0) {
            output += base64_chars[(val >> valb) & 0x3F];
            valb -= 6;
        }
    }
    
    if (valb > -6) {
        output += base64_chars[((val << 8) >> (valb + 8)) & 0x3F];
    }
    
    while (output.length() % 4) {
        output += '=';
    }
    
    return output;
}

String base64Decode(String input) {
    String output = "";
    int val = 0;
    int valb = -8;
    
    for (unsigned int i = 0; i < input.length(); i++) {
        char c = input[i];
        if (c == '=') break;
        
        int index = -1;
        for (int j = 0; j < 64; j++) {
            if (base64_chars[j] == c) {
                index = j;
                break;
            }
        }
        
        if (index == -1) continue;
        
        val = (val << 6) + index;
        valb += 6;
        
        if (valb >= 0) {
            output += char((val >> valb) & 0xFF);
            valb -= 8;
        }
    }
    
    return output;
}

void base64Command(String operation, String text) {
    if (operation == "encode") {
        String encoded = base64Encode(text);
        printLine("Encoded: " + encoded);
    } else if (operation == "decode") {
        String decoded = base64Decode(text);
        printLine("Decoded: " + decoded);
    } else {
        printLine("Usage: base64 encode <text>");
        printLine("       base64 decode <text>");
    }
}


void showHelp() {
    printLine("MiniOS Command Help");
    printLine("");
    printLine("  help file     - File commands");
    printLine("  help system   - System commands");
    printLine("  help network  - Network commands");
    printLine("  help utils    - Utility commands");
    printLine("  help time     - Time commands");
    printLine("  help display  - Display commands");
    printLine("  help os       - OS management");
}

void showHelpFile() {
    printLine("File Commands:");
    printLine("  write <file> <text>   - Write text");
    printLine("  append <file> <text>  - Append text");
    printLine("  read <file>           - Read file");
    printLine("  delete <file>         - Delete file (alias: rm)");
    printLine("  ls                    - List files (alias: dir)");
    printLine("  mv <old> <new>        - Rename file (alias: rename)");
    printLine("  cp <src> <dst>        - Copy file (alias: copy)");
}

void showHelpSystem() {
    printLine("System Commands:");
    printLine("  mem       - Memory info (alias: free)");
    printLine("  uptime    - System uptime");
    printLine("  reboot    - Restart device (alias: restart)");
    printLine("  fetch     - System info (alias: neofetch)");
    printLine("  os        - OS logo (alias: logo)");
    printLine("  version   - OS version (alias: ver)");
    printLine("  clear     - Clear display (alias: cls)");
    printLine("  history   - Command history (alias: hist)");
}

void showHelpNetwork() {
    printLine("Network Commands:");
    printLine("  wifi              - Connect to WiFi");
    printLine("  disconnect        - Disconnect WiFi");
    printLine("  scanwifi          - Scan networks");
    printLine("  ifconfig          - Network info");
    printLine("  ping <host>       - Ping host");
    printLine("  nslookup <host>   - DNS lookup");
    printLine("  curl <url>        - Fetch URL");
    printLine("  curl -v <url>     - Verbose mode");
}

void showHelpUtils() {
    printLine("Utility Commands:");
    printLine("  calc <expr>                 - Calculator");
    printLine("  hex <number>                - Dec to hex");
    printLine("  bin <number>                - Dec to bin");
    printLine("  base64 encode <text>        - Encode Base64");
    printLine("  base64 decode <text>        - Decode Base64");
    printLine("  graph <expression> <colour> - Graph function");
    printLine("  echo <text>                 - Print text");
}

void showHelpTime() {
    printLine("Time Commands:");
    printLine("  time            - Current time");
    printLine("  synctime        - Sync with NTP");
    printLine("  calendar        - Show calendar");
    printLine("  timer <sec>     - Countdown timer");
    printLine("  stopwatch       - Elapsed timer");
    printLine("  alarm <HH:MM>   - Set alarm");
}

void showHelpOS() {
    printLine("OS Commands:");
    printLine("  ps / processes - List processes");
    printLine("  sysstat / stat - System stats");
    printLine("  kill <pid>     - Kill process");
}

void showHelpDisplay() {
    printLine("Display Commands:");
    printLine("  themes          - List themes");
    printLine("  theme <n>       - Select theme");
    printLine("  screensaver <n> - Run screensaver");
    printLine("  pug             - Show pug image");
}


int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    if (op == '^') return 3;
    return 0;
}

bool isRightAssociative(char op) {
    return (op == '^');
}

float applyOp(float a, float b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': 
            if (b == 0) {
                printLine("Error: Division by zero");
                return 0;
            }
            return a / b;
        case '%': 
            if (b == 0) {
                printLine("Error: Modulo by zero");
                return 0;
            }
            return (int)a % (int)b;
        case '^': return pow(a, b);
    }
    return 0;
}

bool isFunction(const String& expr, int pos, const String& func) {
    int len = func.length();
    if (pos + len > expr.length()) return false;
    return expr.substring(pos, pos + len).equalsIgnoreCase(func);
}

String getFunctionName(const String& expr, int pos) {
    String functions[] = {
        "sqrt", "sin", "cos", "tan", "asin", "acos", "atan",
        "sinh", "cosh", "tanh", "log", "ln", "exp",
        "abs", "ceil", "floor", "round"
    };
    for (int i = 0; i < 17; i++) {
        if (isFunction(expr, pos, functions[i])) {
            return functions[i];
        }
    }
    return "";
}

float applyFunction(const String& func, float value) {
    if (func == "sqrt") return sqrt(value);
    if (func == "sin") return sin(value);
    if (func == "cos") return cos(value);
    if (func == "tan") return tan(value);
    if (func == "asin") return asin(value);
    if (func == "acos") return acos(value);
    if (func == "atan") return atan(value);
    if (func == "sinh") return sinh(value);
    if (func == "cosh") return cosh(value);
    if (func == "tanh") return tanh(value);
    if (func == "log") return log10(value);
    if (func == "ln") return log(value);
    if (func == "exp") return exp(value);
    if (func == "abs") return abs(value);
    if (func == "ceil") return ceil(value);
    if (func == "floor") return floor(value);
    if (func == "round") return round(value);
    return value;
}

void calc(String expression) {
    expression.replace(" ", "");
    expression.replace("pi", String(PI, 10));
    expression.replace("e", String(E, 10));
    
    if (expression.length() == 0) {
        printLine("Error: Empty expression");
        return;
    }
    
    float values[50];
    char ops[50];
    String functions[50];
    int vTop = -1;
    int oTop = -1;
    int fTop = -1;
    
    int n = expression.length();
    
    for (int i = 0; i < n; i++) {
        String funcName = getFunctionName(expression, i);
        if (funcName.length() > 0) {
            i += funcName.length();
            functions[++fTop] = funcName;
            continue;
        }
        
        if (expression[i] == '(') {
            ops[++oTop] = '(';
        }
        else if (expression[i] == ')') {
            while (oTop >= 0 && ops[oTop] != '(') {
                float b = values[vTop--];
                float a = values[vTop--];
                char op = ops[oTop--];
                values[++vTop] = applyOp(a, b, op);
            }
            if (oTop >= 0) oTop--;
            
            if (fTop >= 0) {
                float val = values[vTop--];
                val = applyFunction(functions[fTop--], val);
                values[++vTop] = val;
            }
        }
        else if (isdigit(expression[i]) || expression[i] == '.') {
            float num = 0;
            float decimal = 0;
            bool hasDecimal = false;
            int decimalPlaces = 0;
            
            while (i < n && (isdigit(expression[i]) || expression[i] == '.')) {
                if (expression[i] == '.') {
                    hasDecimal = true;
                } else {
                    if (hasDecimal) {
                        decimal = decimal * 10 + (expression[i] - '0');
                        decimalPlaces++;
                    } else {
                        num = num * 10 + (expression[i] - '0');
                    }
                }
                i++;
            }
            i--;
            
            if (hasDecimal && decimalPlaces > 0) {
                num += decimal / pow(10, decimalPlaces);
            }
            
            values[++vTop] = num;
        }
        else if (expression[i] == '-' && 
                (i == 0 || expression[i-1] == '(' || expression[i-1] == '+' || 
                 expression[i-1] == '-' || expression[i-1] == '*' || 
                 expression[i-1] == '/' || expression[i-1] == '^')) {
            values[++vTop] = 0;
            ops[++oTop] = '-';
        }
        else if (expression[i] == '+' || expression[i] == '-' || 
                 expression[i] == '*' || expression[i] == '/' || 
                 expression[i] == '%' || expression[i] == '^') {
            while (oTop >= 0 && ops[oTop] != '(' && 
                   (precedence(ops[oTop]) > precedence(expression[i]) ||
                    (precedence(ops[oTop]) == precedence(expression[i]) && 
                     !isRightAssociative(expression[i])))) {
                float b = values[vTop--];
                float a = values[vTop--];
                char op = ops[oTop--];
                values[++vTop] = applyOp(a, b, op);
            }
            ops[++oTop] = expression[i];
        }
        else {
            printLine("Error: Invalid character '" + String(expression[i]) + "'");
            return;
        }
    }
    
    while (oTop >= 0) {
        if (ops[oTop] == '(') {
            printLine("Error: Mismatched parentheses");
            return;
        }
        float b = values[vTop--];
        float a = values[vTop--];
        char op = ops[oTop--];
        values[++vTop] = applyOp(a, b, op);
    }
    
    if (vTop != 0) {
        printLine("Error: Invalid expression");
        return;
    }
    
    float result = values[vTop];
    
    if (result == (int)result && abs(result) < 1000000) {
        printLine("Result: " + String((int)result));
    } else {
        printLine("Result: " + String(result, 6));
    }
}




void showMem() {
    uint32_t fbytes = ESP.getFreeHeap();
    uint32_t mibytes = ESP.getMinFreeHeap();
    uint32_t mabytes = ESP.getMaxAllocHeap();
    float fkb = fbytes / 1024.0;
    float mikb = mibytes / 1024.0;
    float makb = mabytes / 1024.0;

    printLine("Free Heap: " + String(ESP.getFreeHeap()) + " bytes" + " ("+String(fkb, 2)+" KB)");
    printLine("Min Free Heap: " + String(ESP.getMinFreeHeap()) + " bytes" + " ("+String(mikb, 2)+" KB)");
    printLine("Max Alloc Heap: " + String(ESP.getMaxAllocHeap()) + " bytes" + " ("+String(makb, 2)+" KB)");
}


void showUptime() {
    unsigned long s = millis() / 1000;
    unsigned long h = s / 3600;
    unsigned long m = (s % 3600) / 60;
    unsigned long sec = s % 60;
    printLine("Uptime: " + String(h) + "h " + String(m) + "m " + String(sec) + "s");
}

void doReboot() {
    printLine("Rebooting...");
    vTaskDelay(100 / portTICK_PERIOD_MS);
    ESP.restart();
}

void showChipInfo() {
    printLine("Chip Model: " + String(ESP.getChipModel()));
    printLine("Chip Cores: " + String(ESP.getChipCores()));
    printLine("Chip Revision: " + String(ESP.getChipRevision()));
}
void showCPUInfo() {
    printLine(
        "CPU: " +
        String(ESP.getCpuFreqMHz()) + " MHz"
    );
}
void showFlashInfo() {
    uint32_t flashSize = ESP.getFlashChipSize() / 1024 / 1024;
    printLine(
        "Flash: " +
        String(flashSize) + " MB"
    );
    printLine("Flash Speed: " + String(ESP.getFlashChipSpeed() / 1000000) + " MHz");
}
void showWiFiInfo() {
    if (WiFi.isConnected()) {
        printLine("WiFi RSSI: " + String(WiFi.RSSI()) + " dBm");
        printLine("WiFi Channel: " + String(WiFi.channel()));
        printLine("MAC: " + WiFi.macAddress());
    } else {
        printLine("WiFi: Disconnected");
    }
}

void fetch() {
    showLogo();
    printLine("");
    showUptime();
    showMem();
    showChipInfo();
    showFlashInfo();
    showCPUInfo();
    showWiFiInfo();
    
    printLine("");
    
    int startX = 5;
    int startY = tft.getCursorY() + 5; 
    int blockWidth = 15;
    int blockHeight = 10;

    uint16_t colors1[8] = {0x0000,0x7800,0x03E0,0x7BE0,0x0010,0x780F,0x03EF,0xC618};
    for (int i=0;i<8;i++) tft.fillRect(startX + i*blockWidth, startY, blockWidth, blockHeight, colors1[i]);

    
    startY += blockHeight;
    uint16_t colors2[8] = {0x4208,0xF800,0x07E0,0xFFE0,0x001F,0xF81F,0x07FF,0xFFFF};
    for (int i=0;i<8;i++) tft.fillRect(startX + i*blockWidth, startY, blockWidth, blockHeight, colors2[i]);

    
    startY += blockHeight;
    tft.setCursor(startX, startY + 5);
    currentCursorY = startY; 


    }

void echoCommand(String text) {
    printLine(text);
}


struct CommandArgs {
    String cmd;
    String arg1;
    String arg2;
    String rest;
};

CommandArgs parseCommand(String input) {
    CommandArgs args;
    input.trim();
    
    int firstSpace = input.indexOf(' ');
    if (firstSpace == -1) {
        args.cmd = input;
        return args;
    }
    
    args.cmd = input.substring(0, firstSpace);
    String remainder = input.substring(firstSpace + 1);
    remainder.trim();
    
    int secondSpace = remainder.indexOf(' ');
    if (secondSpace == -1) {
        args.arg1 = remainder;
        return args;
    }
    
    args.arg1 = remainder.substring(0, secondSpace);
    args.rest = remainder.substring(secondSpace + 1);
    args.rest.trim();
    
    int thirdSpace = args.rest.indexOf(' ');
    if (thirdSpace != -1) {
        args.arg2 = args.rest.substring(0, thirdSpace);
        args.rest = args.rest.substring(thirdSpace + 1);
        args.rest.trim();
    } else if (args.rest.length() > 0) {
        args.arg2 = args.rest;
        args.rest = "";
    }
    
    return args;
}

void runCommand(String cmd) {
    cmd.trim();
    
    if (cmd.length() == 0){
        if(currentCursorY>=MAX_Y){
            clearScreen();
            tft.print("> ");
        }
        return;
    }
    
    addToHistory(cmd);
    
    CommandArgs args = parseCommand(cmd);
    String baseCmd = args.cmd;
    baseCmd.toLowerCase();
    
    if (baseCmd == "write") {
        if (args.arg1.length() == 0 || args.rest.length() == 0) {
            printLine("Usage: write <filename> <text>");
            return;
        }
        writeFile(args.arg1, args.rest);
    }
    else if (baseCmd == "append") {
        if (args.arg1.length() == 0 || args.rest.length() == 0) {
            printLine("Usage: append <filename> <text>");
            return;
        }
        appendFile(args.arg1, args.rest);
    }
    else if (baseCmd == "read") {
        if (args.arg1.length() == 0) {
            printLine("Usage: read <filename>");
            return;
        }
        readFile(args.arg1);
    }
    else if (baseCmd == "delete" || baseCmd == "rm") {
        if (args.arg1.length() == 0) {
            printLine("Usage: delete <filename>");
            return;
        }
        deleteFile(args.arg1);
    }
    else if (baseCmd == "ls" || baseCmd == "dir") {
        listFiles();
    }
    else if (baseCmd == "mv" || baseCmd == "rename") {
        if (args.arg1.length() == 0 || args.arg2.length() == 0) {
            printLine("Usage: mv <old> <new>");
            return;
        }
        renameFile(args.arg1, args.arg2);
    }
    else if (baseCmd == "cp" || baseCmd == "copy") {
        if (args.arg1.length() == 0 || args.arg2.length() == 0) {
            printLine("Usage: cp <src> <dst>");
            return;
        }
        copyFile(args.arg1, args.arg2);
    }
    else if (baseCmd == "wifi") {
        if (args.arg1 == "disconnect") {
            disconnectWiFi();
        } else {
            connectWiFi();
        }
    }
    else if (baseCmd == "disconnect") {
        disconnectWiFi();
    }
    else if (baseCmd == "scanwifi" || baseCmd == "wifiscan") {
        scanWiFi();
    }
    else if (baseCmd == "ifconfig" || baseCmd == "netinfo" || baseCmd == "ipconfig") {
        showNetworkInfo();
    }
    else if (baseCmd == "curl") {
        if (args.arg1.length() == 0) {
            printLine("Usage: curl [-v] <url>");
            return;
        }
        
        if (args.arg1 == "-v") {
            if (args.rest.length() == 0) {
                printLine("Usage: curl -v <url>");
                return;
            }
            curlURLVerbose(args.rest);
        } else {
            curlURL(args.arg1 + (args.rest.length() > 0 ? " " + args.rest : ""));
        }
    }
    else if (baseCmd == "ping") {
        if (args.arg1.length() == 0) {
            printLine("Usage: ping <host>");
            return;
        }
        pingHost(args.arg1);
    }
    else if (baseCmd == "nslookup" || baseCmd == "dns") {
        if (args.arg1.length() == 0) {
            printLine("Usage: nslookup <host>");
            return;
        }
        dnsLookup(args.arg1);
    }
    else if (baseCmd == "mem" || baseCmd == "memory") {
        showMem();
    }
    else if (baseCmd == "uptime") {
        showUptime();
    }
    else if (baseCmd == "reboot" || baseCmd == "restart") {
        doReboot();
    }
    else if (baseCmd == "fetch" || baseCmd == "neofetch" || baseCmd == "fastfetch" ) {
        fetch();
    }
    else if (baseCmd == "os" || baseCmd == "logo") {
        showLogo();
    }
    else if (baseCmd == "version" || baseCmd == "ver") {
        showVersion();
    }
    else if (baseCmd == "clear" || baseCmd == "cls") {
        clearScreen();
        tft.print("> ");
    }
    else if (baseCmd == "history" || baseCmd == "hist") {
        showHistory();
    }
    else if (baseCmd == "ps" || baseCmd == "processes" || baseCmd == "top") {
        listProcesses();
    }
    else if (baseCmd == "sysstat" || baseCmd == "stat") {
        showSystemStats();
    }
    else if (baseCmd == "kill") {
        if (args.arg1.length() == 0) {
            printLine("Usage: kill <pid>");
            return;
        }
        int pid = args.arg1.toInt();
        if (pid <= 0) {
            printLine("Invalid PID");
            return;
        }
        killProcess(pid);
    }
    else if (baseCmd == "echo") {
        echoCommand(args.arg1 + (args.rest.length() > 0 ? " " + args.rest : ""));
    }
    else if (baseCmd == "calc") {
        if (args.arg1.length() == 0) {
            printLine("Usage: calc <expression>");
            return;
        }
        calc(args.arg1 + (args.rest.length() > 0 ? " " + args.rest : ""));
    }
    else if (baseCmd == "hex") {
        if (args.arg1.length() == 0) {
            printLine("Usage: hex <number>");
            return;
        }
        hexCommand(args.arg1);
    }
    else if (baseCmd == "bin") {
        if (args.arg1.length() == 0) {
            printLine("Usage: bin <number>");
            return;
        }
        binCommand(args.arg1);
    }
    else if (baseCmd == "base64") {
        if (args.arg1.length() == 0) {
            printLine("Usage: base64 encode <text>");
            printLine("       base64 decode <text>");
            return;
        }
        
        String operation = args.arg1;
        String text = args.arg2;
        if (args.rest.length() > 0) {
            text = args.arg2 + " " + args.rest;
        }
        
        if (text.length() == 0) {
            printLine("Usage: base64 encode <text>");
            printLine("       base64 decode <text>");
            return;
        }
        
        base64Command(operation, text);
    }   
    else if (baseCmd == "time" || baseCmd == "date") {
        printLine(getTime());
    }
    else if (baseCmd == "synctime" || baseCmd == "ntpupdate") {
        syncTime();
    }
    else if (baseCmd == "calendar" || baseCmd == "cal") {
        showCalendar();
    }
    else if (baseCmd == "timer") {
        if (args.arg1.length() == 0) {
            printLine("Usage: timer <seconds>");
            return;
        }
        int seconds = args.arg1.toInt();
        if (seconds <= 0) {
            printLine("Invalid time");
            return;
        }
        timerCommand(seconds);
    }
    else if (baseCmd == "stopwatch" || baseCmd == "sw") {
        stopwatchCommand();
    }
    else if (baseCmd == "alarm") {
        if (args.arg1.length() == 0) {
            if (systemAlarm.active) {
                printLine("Alarm set for " + String(systemAlarm.hour) + ":" + 
                         (systemAlarm.minute < 10 ? "0" : "") + String(systemAlarm.minute));
            } else {
                printLine("No alarm set.");
            }
        } else {
            setAlarm(args.arg1);
        }
    }
    else if (baseCmd == "themes") {
        listThemes();
    }
    else if (baseCmd == "theme") {
        if (args.arg1.length() == 0) {
            printLine("Usage: theme <number>");
            listThemes();
            return;
        }
        setTheme(args.arg1);
    }
    else if (baseCmd == "pug") {
        displayPug();
    }
    else if (baseCmd == "screensaver" || baseCmd == "ss") {
        if (args.arg1.length() == 0) {
            printLine("Usage: screensaver <mode>");
            printLine("Available modes: 1-7");
            return;
        }
        
        int mode = args.arg1.toInt();
        if (mode < 1 || mode > 7) {
            printLine("Invalid mode. Use 1-7.");
            return;
        }
        
        screensaver(mode);
    }
    else if (baseCmd == "graph" || baseCmd == "plot") {
        if (args.arg1.length() == 0) {
            printLine("Usage: graph <expression> [color]");
            printLine("Example: graph sin(x) red");
            return;
        }
        String color = args.arg2.length() > 0 ? args.arg2 : "blue";
        funcToGraph(args.arg1, color);
    }
    else if (baseCmd == "help" || baseCmd == "h") {
        if (args.arg1.length() == 0) {
            showHelp();
        } else if (args.arg1 == "file") {
            showHelpFile();
        } else if (args.arg1 == "system") {
            showHelpSystem();
        } else if (args.arg1 == "network") {
            showHelpNetwork();
        } else if (args.arg1 == "utils") {
            showHelpUtils();
        } else if (args.arg1 == "time") {
            showHelpTime();
        } else if (args.arg1 == "display") {
            showHelpDisplay();
        } else if (args.arg1 == "os") {
            showHelpOS();
        } else {
            printLine("Unknown help topic: " + args.arg1);
            showHelp();
        }
    }
    else {
        printLine("Unknown command: " + baseCmd);
        printLine("Type 'help' for available commands");
    }
}




void processCommand(String args) {
    listProcesses();
}

void showSystemStats() {
    printSystemStats();
}

void killProcessCmd(int pid) {
    killProcess(pid);
}
