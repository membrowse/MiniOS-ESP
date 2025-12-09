#include "commands.h"
#include "display.h"
#include "filesystem.h"
#include "network.h"
#include "theme.h"
#include "config.h"

void showVersion() {
    printLine(OS_VERSION);
    printLine("Check: https://github.com/VuqarAhadli");
}

void showHelp() {
    printLine("Commands:");
    printLine(" write <file> <text>");
    printLine(" read <file>");
    printLine(" delete <file>");
    printLine(" version");
    printLine(" time");
    printLine(" synctime");
    printLine(" wifi");
    printLine(" curl <url>");
    printLine(" calc <expression>");
    printLine(" themes");
    printLine(" theme <name|number>");
    printLine(" os");
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
    else if (cmd.startsWith("curl ")) {
        curlURL(cmd.substring(5));
    }
    else if (cmd == "os") {
        showLogo();
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
    else {
        printLine("Unknown command.");
    }
}