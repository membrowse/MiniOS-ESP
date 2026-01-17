#ifndef GRAPHER.H
#define GRAPHER.H

#include <Arduino.h>

bool evaluateWithX(String expression, float xValue, float& result);
void funcToGraph(String expression, String lineColour);

#endif