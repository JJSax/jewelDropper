#pragma once

#include "mergeify.hpp"
#include "keybinds.hpp"

constexpr int gWidth = 10;
constexpr int gHeight = 18;
constexpr int cellSize = 30;
constexpr float foreheadH = cellSize * 4;
extern float musicVolume;

extern VariableGroup dropTime; // time to drop one space
extern VariableGroup holdDropTime;
extern VariableGroup holdShiftTime;
extern VariableGroup reduceTime;
extern VariableGroup blinkTime; // will be reduced by dt * 4.  two on/off cycle per second

extern KeyMapper keyMapper;

void loadOptions();