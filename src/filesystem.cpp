#include "filesystem.h"
#include "display.h"
#include <FS.h>
#include <SPIFFS.h>

bool initFilesystem() {
    if (!SPIFFS.begin(true)) {
        printLine("SPIFFS Failed.");
        return false;
    }
    
    
    size_t total = SPIFFS.totalBytes();
    size_t used = SPIFFS.usedBytes();
    printLine("SPIFFS: " + String(used) + "/" + String(total) + " bytes");
    
    return true;
}

void writeFile(String name, String data) {
   
    if (!name.startsWith("/")) {
        name = "/" + name;
    }
    
    File f = SPIFFS.open(name, FILE_WRITE);
    if (!f) {
        printLine("Error opening file.");
        return;
    }
    
    size_t written = f.print(data);
    f.flush();  
    f.close();
    
    if (written > 0) {
        printLine("Written " + String(written) + " bytes.");
    } else {
        printLine("Error: 0 bytes written.");
    }
}

void appendFile(String name, String data) {
    
    if (!name.startsWith("/")) {
        name = "/" + name;
    }
    
    File f = SPIFFS.open(name, FILE_APPEND);
    if (!f) {
        printLine("Error opening file.");
        return;
    }
    
    size_t written = f.print(data);
    f.flush(); 
    f.close();
    
    if (written > 0) {
        printLine("Appended " + String(written) + " bytes.");
    } else {
        printLine("Error: 0 bytes appended.");
    }
}

void readFile(String name) {
    
    if (!name.startsWith("/")) {
        name = "/" + name;
    }
    
    File f = SPIFFS.open(name);
    if (!f) {
        printLine("Error reading file.");
        return;
    }
    
    if (f.available()) {
        printLine("File: " + name);
        while (f.available()) {
            printLine(f.readStringUntil('\n'));
        }
    } else {
        printLine("File is empty.");
    }
    
    f.close();
}

void deleteFile(String name) {
    
    if (!name.startsWith("/")) {
        name = "/" + name;
    }
    
    if (SPIFFS.remove(name)) {
        printLine("File deleted.");
    } else {
        printLine("Error deleting file.");
    }
}

void listFiles() {
    size_t totalBytes = SPIFFS.totalBytes();
    size_t usedBytes = SPIFFS.usedBytes();
    printLine("SPIFFS: " + String(usedBytes) + "/" + String(totalBytes) + " bytes");
    printLine("Files:");
    
    File root = SPIFFS.open("/");
    if (!root) {
        printLine("Failed to open root");
        return;
    }
    
    bool found = false;
    
    while (true) {
        File file = root.openNextFile();
        if (!file) {
            break;
        }
        
        found = true;
        String fileName = String(file.name());
        
        
        if (fileName.startsWith("/")) {
            fileName = fileName.substring(1);
        }
        
        if (fileName.length() > 0) {
            printLine("  " + fileName + " - " + String(file.size()) + " bytes");
        }
        
        file.close();
    }
    
    if (!found) {
        printLine("  (no files)");
    }
    
    root.close();
}

bool renameFile(String oldName, String newName) {
   
    if (!oldName.startsWith("/")) {
        oldName = "/" + oldName;
    }
    if (!newName.startsWith("/")) {
        newName = "/" + newName;
    }
    
    if (SPIFFS.rename(oldName, newName)) {
        printLine("Renamed.");
        return true;
    }
    
    printLine("Rename failed.");
    return false;
}

bool copyFile(String src, String dst) {
    
    if (!src.startsWith("/")) {
        src = "/" + src;
    }
    if (!dst.startsWith("/")) {
        dst = "/" + dst;
    }
    
    File in = SPIFFS.open(src);
    if (!in) {
        printLine("Error reading src file.");
        return false;
    }
    
    File out = SPIFFS.open(dst, FILE_WRITE);
    if (!out) {
        in.close();
        printLine("Error opening dst file.");
        return false;
    }
    
    uint8_t buf[64];
    while (in.available()) {
        int len = in.read(buf, sizeof(buf));
        out.write(buf, len);
    }
    
    in.close();
    out.flush();  
    out.close();
    
    printLine("Copied.");
    return true;
}