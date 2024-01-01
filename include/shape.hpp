#pragma once

#include <raylib.h>
#include <vector>
#include <optional>

#include "options.hpp"
#include "enums.hpp"

extern Texture2D jewel;

class Tile {
public:
	int ox; // offset from host shape x
	int oy; // offset from host shape y
	Color color;

	Tile(int x, int y, Color col) : ox(x), oy(y), color(col) {}

	void rawDraw(int x, int y) const {
		float dx = x * cellSize + 2;
		float dy = y * cellSize + 2;
		float scale = static_cast<float>(cellSize - 4) / jewel.height;
		Vector2 pos { dx, dy };
		DrawRectangle(pos.x, pos.y, cellSize - 4, cellSize - 4, color);
		DrawRectangleLines(pos.x, pos.y, cellSize - 4, cellSize - 4, BLACK);
		DrawTextureEx(jewel, pos, 0, scale, GRAY);
	}
};

class Shape {
public:
	short int x, y;
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
	int getLeft();
	int getRight();
};

Shape *randomPiece(std::optional<Tile> history[][gHeight]);