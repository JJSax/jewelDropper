#pragma once

extern "C" {
#include <raylib.h>
}
#include <optional>
#include <vector>

constexpr int gWidth = 10;
constexpr int gHeight = 18;
constexpr int cellSize = 30;

enum shiftDir {
	LEFT = -1,
	RIGHT = 1
};

struct tile {
	int ox; // offset from host shape x
	int oy; // offset from host shape y
	Color color;

	tile(int x, int y, Color col) :
		ox(x),
		oy(y),
		color(col) {
		// Additional initialization if needed
	}

	void rawDraw(int x, int y) const {
		DrawRectangle(x * cellSize + 2, y * cellSize + 2, cellSize - 4, cellSize - 4, color);
		DrawRectangleLines(x * cellSize + 2, y * cellSize + 2, cellSize - 4, cellSize - 4, BLACK);
	}
};

class shape {
public:
	int x, y;
	bool settled;
	std::vector<tile> tiles;
	int settledY;
	int rotation = 0;
	float midx, midy;

	shape();
	bool step(std::optional<tile> history[][gHeight]);
	void shift(shiftDir LR, std::optional<tile> history[][gHeight]);
	void pivot(std::optional<tile> history[][gHeight]);
	void gravity(std::optional<tile> history[][gHeight]);
	void draw();
};

class game {
	std::optional<tile> history[gWidth][gHeight];
	bool removeRowIfCompleted(int y);
public:
	game();
	shape *currentPiece[4];
	shape *holding;
	int score;
	int tilesDropped = 0;
	int tilesDroppedHolding;
	bool isOccupied(int x, int y);
	const tile& tileAt(int x, int y);
	bool step(); // also settles pieces and generates a new one when doing so
	void shift(shiftDir LR);
	void pivot();
	void holdSwap();
};