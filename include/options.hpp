#pragma once

#include "mergeify.hpp"

constexpr int gWidth = 10;
constexpr int gHeight = 18;
constexpr int cellSize = 30;
const float foreheadH = cellSize * 4;
float musicVolume = 0.3f;

extern VariableGroup dropTime; // time to drop one space
extern VariableGroup holdDropTime;
extern VariableGroup holdShiftTime;
extern VariableGroup reduceTime;
extern VariableGroup blinkTime; // will be reduced by dt * 4.  two on/off cycle per second