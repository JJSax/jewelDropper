#pragma once

#include <raylib.h>
#include <optional>
#include <vector>
#include <set>

constexpr int gWidth = 10;
constexpr int gHeight = 18;
constexpr int cellSize = 30;

enum shiftDir {
	LEFT = -1,
	RIGHT = 1
};

class Tile {
public:
	int ox; // offset from host shape x
	int oy; // offset from host shape y
	Color color;

	Tile(int x, int y, Color col) : ox(x), oy(y), color(col) {}

	void rawDraw(int x, int y) const {
		DrawRectangle(x * cellSize + 2, y * cellSize + 2, cellSize - 4, cellSize - 4, color);
		DrawRectangleLines(x * cellSize + 2, y * cellSize + 2, cellSize - 4, cellSize - 4, BLACK);
	}
};
class Shape {
public:
	int x, y;
	bool settled;
	std::vector<Tile> tiles;
	int settledY;
	int rotation = 0;
	const float midx, midy;

	Shape(float midX, float midY);
	bool step(std::optional<Tile> history[][gHeight]);
	bool shift(shiftDir LR, std::optional<Tile> history[][gHeight]);
	void pivot(std::optional<Tile> history[][gHeight]);
	void gravity(std::optional<Tile> history[][gHeight]);
	void draw();
};

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