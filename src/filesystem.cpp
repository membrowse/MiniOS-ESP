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

void appendFile(String name, String data) {
    File f = SPIFFS.open("/" + name, FILE_APPEND);
    if (!f) {
        printLine("Error appending file.");
        return;
    }
    f.print(data);
    f.close();
    printLine("Appended.");
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

void listFiles() {
    File root = SPIFFS.open("/");
    if (!root) {
        printLine("Failed to open root");
        return;
    }
    root.rewindDirectory(); 
    bool found = false;
    while (true) {
        File file = root.openNextFile();
        if (!file) break;  
        found = true;
        printLine(String(file.name()) + "  " + String(file.size()) + " bytes");
    }

    if (!found) {
        printLine("No files.");
    }

    root.close();
}



bool renameFile(String oldName, String newName) {
    if (SPIFFS.rename(String("/") + oldName, String("/") + newName)) {
        printLine("Renamed.");
        return true;
    }
    printLine("Rename failed.");
    return false;
}

bool copyFile(String src, String dst) {
    File in = SPIFFS.open("/" + src);
    if (!in) { printLine("Error reading src file."); return false; }
    File out = SPIFFS.open("/" + dst, FILE_WRITE);
    if (!out) { in.close(); printLine("Error opening dst file."); return false; }
    uint8_t buf[64];
    while (in.available()) {
        int len = in.read(buf, sizeof(buf));
        out.write(buf, len);
    }
    in.close();
    out.close();
    printLine("Copied.");
    return true;
}