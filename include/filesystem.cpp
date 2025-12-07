#include "filesystem.h"
#include "display.h"
#include <FS.h>
#include <SPIFFS.h>

bool initFilesystem() {
    if (!SPIFFS.begin(true)) {
        printLine("SPIFFS Failed.");
        return false;
    }
    return true;
}

void writeFile(String name, String data) {
    File f = SPIFFS.open("/" + name, FILE_WRITE);
    if (!f) {
        printLine("Error writing file.");
        return;
    }
    f.print(data);
    f.close();
    printLine("Written.");
}

void readFile(String name) {
    File f = SPIFFS.open("/" + name);
    if (!f) {
        printLine("Error reading file.");
        return;
    }
    
    if(f.available()){ 
        printLine("File: " + name);
        while (f.available()) {
            printLine(f.readStringUntil('\n'));
        }
    }
    f.close();
}

void deleteFile(String name) {
    if (SPIFFS.remove("/" + name))
        printLine("File deleted.");
    else
        printLine("Error deleting file.");
}