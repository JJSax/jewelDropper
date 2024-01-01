
#include "options.hpp"

VariableGroup dropTime = 0.75f; // time to drop one space
VariableGroup holdDropTime = 0.06f;
VariableGroup holdShiftTime = 0.05f;
VariableGroup reduceTime = 1.0f;
VariableGroup blinkTime = 4.0f; // will be reduced by dt * 4.  two on/off cycle per second

float musicVolume = 0.1f;

KeyMapper keyMapper;

void loadOptions() {
	keyMapper.addMapping(KEY_A, 	Keybinds::SHIFTLEFT);
	keyMapper.addMapping(KEY_LEFT, 	Keybinds::SHIFTLEFT);
	keyMapper.addMapping(KEY_D, 	Keybinds::SHIFTRIGHT);
	keyMapper.addMapping(KEY_RIGHT, Keybinds::SHIFTRIGHT);
	keyMapper.addMapping(KEY_UP,	Keybinds::PIVOT);
	keyMapper.addMapping(KEY_W,		Keybinds::PIVOT);
	keyMapper.addMapping(KEY_DOWN,	Keybinds::STEP);
	keyMapper.addMapping(KEY_S,		Keybinds::STEP);
	keyMapper.addMapping(KEY_SPACE,	Keybinds::SLAM);
	keyMapper.addMapping(KEY_C,		Keybinds::SWAPHOLD);
}