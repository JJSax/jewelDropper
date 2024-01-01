
#include "mergeify.hpp"

constexpr int gWidth = 10;
constexpr int gHeight = 18;
constexpr int cellSize = 30;

VariableGroup dropTime = 0.75f; // time to drop one space
VariableGroup holdDropTime = 0.06f;
VariableGroup holdShiftTime = 0.05f;
VariableGroup reduceTime = 1.0f;
VariableGroup blinkTime = 4.0f; // will be reduced by dt * 4.  two on/off cycle per second