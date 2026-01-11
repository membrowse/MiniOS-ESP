#include "commands.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "config.h"
#include "pug.h"
#include <esp_system.h>

void showVersion() {
    printLine(OS_VERSION);
    printLine("Check: https://github.com/VuqarAhadli");
}

void showHelp() {
    const char* commands[] = {
        "write <file> <text>   - Write text to a file",
        "append <file> <text>  - Append text to a file",
        "read <file>           - Read a file",
        "delete <file>         - Delete a file",
        "ls                    - List files",
        "mv <old> <new>        - Rename a file",
        "cp <src> <dst>        - Copy a file",
        "version               - Show version",
        "time                  - Show current time",
        "synctime              - Sync time with server",
        "wifi                  - Show WiFi info",
        "scanwifi              - Scan nearby WiFi networks",
        "curl <url>            - Fetch URL",
        "ping <host>           - Ping host (3 times)",
        "mem                   - Show free heap memory",
        "uptime                - Show time since boot",
        "reboot                - Reboot device",
        "calc <expression>     - Evaluate expression",
        "themes                - List available themes",
        "theme <name|number>   - Select a theme",
        "os                    - Show OS info",
        "pug                   - Display pug image",
        "screensaver           - Display screensaver"
    };

    printLine("Commands:\n");

    const int numCommands = sizeof(commands) / sizeof(commands[0]);
    for (int i = 0; i < numCommands; i++) {
        printLine(commands[i]);
    }
}


int precedence(char op) {
    if (op == '+' || op == '-') return 1;
    if (op == '*' || op == '/' || op == '%') return 2;
    return 0;
}

int ope(int a, int b, char op) {
    switch (op) {
        case '+': return a + b;
        case '-': return a - b;
        case '*': return a * b;
        case '/': return b ? a / b : 0;
        case '%': return b ? a % b : 0;
    }
    return 0;
}

void calc(String expression) {
    int values[50];
    char ops[50];
    int vTop = -1;
    int oTop = -1;
    
    int n = expression.length();
    
    for(int i = 0; i < n; i++) {
        if(expression[i] == ' ') continue;
        
        if(isdigit(expression[i])) {
            int num = 0;
            while(i < n && isdigit(expression[i])) {
                num = num * 10 + (expression[i] - '0');
                i++;
            }
            i--;
            values[++vTop] = num;
        }
        else if(expression[i]=='+' || expression[i]=='-' || 
                expression[i]=='*' || expression[i]=='/' || expression[i]=='%') {
            while(oTop >= 0 && precedence(ops[oTop]) >= precedence(expression[i])) {
                int b = values[vTop--];
                int a = values[vTop--];
                char op = ops[oTop--];
                values[++vTop] = ope(a, b, op);
            }
            ops[++oTop] = expression[i];
        }
    }
    
    while(oTop >= 0) {
        int b = values[vTop--];
        int a = values[vTop--];
        char op = ops[oTop--];
        values[++vTop] = ope(a, b, op);
    }
    
    int result = values[vTop];
    printLine("Result: " + String(result));
}

void showMem() {
    uint32_t bytes = ESP.getFreeHeap();
    float kb = bytes / 1024.0;

    printLine(
        "Free heap: " +
        String(bytes) + " bytes (" +
        String(kb, 1) + " KB)"
    );
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
    delay(100);
    ESP.restart();
}

void runCommand(String cmd) {
    cmd.trim();
    if (cmd.startsWith("write ")) {
        int sp = cmd.indexOf(' ', 6);
        if (sp == -1) {
            printLine("Usage: write filename text");
            return;
        }
        String name = cmd.substring(6, sp);
        String data = cmd.substring(sp + 1);
        writeFile(name, data);
    }
    else if (cmd.startsWith("append ")) {
        int sp = cmd.indexOf(' ', 7);
        if (sp == -1) {
            printLine("Usage: append filename text");
            return;
        }
        String name = cmd.substring(7, sp);
        String data = cmd.substring(sp + 1);
        appendFile(name, data);
    }
    else if (cmd == "ls") {
        listFiles();
    }
    else if (cmd.startsWith("mv ")) {
        int sp = cmd.indexOf(' ', 3);
        if (sp == -1) { printLine("Usage: mv oldname newname"); return; }
        String a = cmd.substring(3, sp);
        String b = cmd.substring(sp + 1);
        renameFile(a, b);
    }
    else if (cmd.startsWith("cp ")) {
        int sp = cmd.indexOf(' ', 3);
        if (sp == -1) { printLine("Usage: cp src dst"); return; }
        String a = cmd.substring(3, sp);
        String b = cmd.substring(sp + 1);
        copyFile(a, b);
    }
    else if (cmd.startsWith("read ")) {
        readFile(cmd.substring(5));
    }
    else if (cmd.startsWith("delete ")) {
        deleteFile(cmd.substring(7));
    }
    else if (cmd == "version") {
        showVersion();
    }
    else if (cmd == "time") {
        printLine(getTime());
    }
    else if (cmd == "synctime") {
        syncTime();
    }
    else if (cmd == "wifi") {
        connectWiFi();
    }
    else if (cmd == "scanwifi") {
        scanWiFi();
    }
    else if (cmd.startsWith("curl ")) {
        curlURL(cmd.substring(5));
    }
    else if (cmd.startsWith("ping ")) {
        pingHost(cmd.substring(5));
    }
    else if (cmd == "os") {
        showLogo();
    }
    else if (cmd == "mem") {
        showMem();
    }
    else if (cmd == "uptime") {
        showUptime();
    }
    else if (cmd == "reboot") {
        doReboot();
    }
    else if (cmd.startsWith("calc ")) {
        calc(cmd.substring(5));
    }
    else if (cmd == "themes") {
        listThemes();
    }
    else if (cmd.startsWith("theme ")) {
        setTheme(cmd.substring(6));
    }
    else if (cmd == "help") {
        showHelp();
    }
    else if (cmd=="pug") {
        displayPug();
    }
    else if (cmd=="screensaver") {
        screensaver();
    }
    else {
        printLine("Unknown command.");
    }
}