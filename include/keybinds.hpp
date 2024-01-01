#pragma once

#include <raylib.h>
#include <vector>
#include <map>

enum class Keybinds {
	SHIFTLEFT,
	SHIFTRIGHT,
	STEP,
	SLAM,
	SWAPHOLD,
	PIVOT
};

class KeyMapper {
public:
	void addMapping(KeyboardKey key, Keybinds action) {
		if (keyMap[action].empty())
			keyMap[action] = std::vector<KeyboardKey>();
		keyMap[action].emplace_back(key);
	}

	bool IsKeybindPressed(Keybinds action) {
		for (KeyboardKey key : keyMap[action]) {
			if (IsKeyPressed(key)) return true;
		}
		return false;
	}

	bool IsKeybindDown(Keybinds action) {
		for (KeyboardKey key : keyMap[action]) {
			if (IsKeyDown(key)) return true;
		}
		return false;
	}

private:
	std::map<Keybinds, std::vector<KeyboardKey>> keyMap;
};

extern KeyMapper keyMapper;
