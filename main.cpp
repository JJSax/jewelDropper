extern "C" {
#include <raylib.h>
}
#include <cstdio>
#include <vector>

int gWidth = 10;
int gHeight = 18;
int cellSize = 30;

enum shiftDir {
	LEFT = -1,
	RIGHT = 1
};

class tile {
public:
	int ox; // offset from host shape x
	int oy; // offset from host shape y

	tile(int x, int y) {
		ox = x;
		oy = y;
	}
};

class shape {
public:
	int x;
	int y;
	bool settled;
	Color color;
	std::vector<tile> tiles;

	bool step() {
		y++;
		for (int i = 0; i < tiles.size(); i++) {
			int tx = x + tiles[i].ox;
			int ty = y + tiles[i].oy;
			if (ty > gHeight) {
				return true;
			}
			//todo check for collision
			//if collision y--;
			//place
		}
		return false;
	}

	void shift(shiftDir LR) {
		x += LR;
		for (int i = 0; i < tiles.size(); i++) {
			if (x + tiles[i].ox == gWidth | x + tiles[i].ox < 0) 
				x += -LR;
		}
	}

	void draw() {
		for (int i = 0; i < tiles.size(); i++) {
			DrawRectangle((x + tiles[i].ox) * cellSize + 2, (y + tiles[i].oy) * cellSize + 2, cellSize - 4, cellSize - 4, color);
			DrawRectangleLines((x + tiles[i].ox) * cellSize + 2, (y + tiles[i].oy) * cellSize + 2, cellSize - 4, cellSize - 4, BLACK);
		}
	}
};

class tShape : public shape {
public:
	tShape() {
		this->x = 4;
		this->y = 2;
		this->color = RED;
		this->settled = false;
		this->tiles.emplace_back( 0, 0);
		this->tiles.emplace_back(-1, 0);
		this->tiles.emplace_back( 1, 0);
		this->tiles.emplace_back( 0,-1);
	}

	void pivot() {
		// rotation
	}
};

int main() {

	// int windowWidth = 720;
	// int windowHeight = 1080;

	int gWidth = 10;
	int gHeight = 18;

	const float DROPTIME = 0.75;
	float dropTime = DROPTIME; // time to drop one space

	tShape falling;

	InitWindow(gWidth * cellSize, gHeight * cellSize, "Jewel Dropper");

	SetTargetFPS(60);
	while (!WindowShouldClose()) {

		BeginDrawing();
		ClearBackground(BLACK);

		// update

		if (IsKeyPressed(KEY_LEFT)) 
			falling.shift(LEFT);
		if (IsKeyPressed(KEY_RIGHT))
			falling.shift(RIGHT);

		dropTime -= GetFrameTime();
		if (dropTime <= 0) {
			dropTime = DROPTIME;
			// drop tiles
			if (falling.step()) {

			}
		}

		// draw
		for (int y = 0; y < gHeight; y++) {
			for (int x = 0; x < gWidth; x++) {
				DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, WHITE);
			}
		}

		falling.draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}