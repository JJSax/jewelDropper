#pragma once

#include <optional>
#include <set>

#include "options.hpp"
#include "shape.hpp"
#include "enums.hpp"

void loadJD();
void unloadJD();
void updateJD();


enum Gamestate {
	PAUSED,
	UNPAUSED,
	GAMEOVER,
	REDUCINGROWS
};

class Game {
	std::optional<Tile> history[gWidth][gHeight];
	bool rowCompleted(int y);
	bool removeRow(int y);

public:
	Game();
	Shape *currentPiece[4];
	Shape *holding;
	Gamestate state = UNPAUSED;
	int score;
	int tilesDropped = 0;
	int tilesDroppedHolding;
	std::set<int> completedRows;
	bool isOccupied(int x, int y);
	const Tile& tileAt(int x, int y);
	bool step(); // Also settles pieces and generates a new one when doing so
	void shift(shiftDir LR);
	void pivot();
	void holdSwap();
	void gravity();
	void removeCompleted();
};