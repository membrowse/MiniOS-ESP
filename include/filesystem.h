#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <Arduino.h>

bool initFilesystem();
void writeFile(String name, String data);
void readFile(String name);
void deleteFile(String name);

#endif