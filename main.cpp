extern "C" {
#include <raylib.h>
}
#include <cstdio>
#include <optional>
#include <vector>
#include <random>
#include <iostream>
using namespace std;

/*
TODO decide if line completion detection is best in step or main loop or elsewhere
*/

const int gWidth = 10;
const int gHeight = 18;
const int cellSize = 30;

enum shiftDir {
	LEFT = -1,
	RIGHT = 1
};

enum ShapeTypes {
	T_SHAPE,
	O_SHAPE,
	I_SHAPE,
	S_SHAPE,
	Z_SHAPE,
	L_SHAPE,
	J_SHAPE,
	N_SHAPES
};

class tile {
public:
	int ox; // offset from host shape x
	int oy; // offset from host shape y
	Color color;

	tile(int x, int y, Color col) :
		ox(x),
		oy(y),
		color(col) {
		// Additional initialization if needed
	}

	void rawDraw(int x, int y) {
		DrawRectangle(x * cellSize + 2, y * cellSize + 2, cellSize - 4, cellSize - 4, color);
		DrawRectangleLines(x * cellSize + 2, y * cellSize + 2, cellSize - 4, cellSize - 4, BLACK);
	}
};

class shape {
public:
	int x;
	int y;
	bool settled;
	// Color color;
	// ShapeTypes shapeType;
	vector<tile> tiles;

	shape(void) {
		this->x = 4;
		this->y = 2;
		this->settled = false;
	}

	bool step(optional<tile> history[][gHeight]) {
		y++;
		for (int i = 0; i < tiles.size(); i++) {
			int tx = x + tiles[i].ox;
			int ty = y + tiles[i].oy;
			if (ty >= gHeight || history[tx][ty].has_value()) {
				y--;
				return true;
			}
		}
		return false;
	}

	void shift(shiftDir LR, optional<tile> history[][gHeight]) {
		x += LR;
		for (int i = 0; i < tiles.size(); i++) {
			int tx = x + tiles[i].ox;
			int ty = y + tiles[i].oy;
			if (tx == gWidth || tx < 0 || history[tx][ty].has_value())
				x += -LR;
		}
	}

	void pivot() {
		for (int i = 1; i < tiles.size(); i++)
		{
			int c = tiles[i].ox;
			tiles[i].ox = -tiles[i].oy;
			tiles[i].oy =  c;
		}
	}

	void draw() {
		for (int i = 0; i < tiles.size(); i++) {
			tiles[i].rawDraw(x + tiles[i].ox, y + tiles[i].oy);
		}
	}
};

class tShape : public shape {
public:
	tShape() : shape() {
		this->tiles.emplace_back( 0, 0, PURPLE);
		this->tiles.emplace_back(-1, 0, PURPLE);
		this->tiles.emplace_back( 1, 0, PURPLE);
		this->tiles.emplace_back( 0,-1, PURPLE);
	}
};

class oShape : public shape {
public:
	oShape() : shape() {
		this->tiles.emplace_back( 0, 0, YELLOW);
		this->tiles.emplace_back( 1, 0, YELLOW);
		this->tiles.emplace_back( 0, 1, YELLOW);
		this->tiles.emplace_back( 1, 1, YELLOW);
	}
};

class iShape : public shape {
public:
	iShape() : shape() {
		this->tiles.emplace_back( 0, 0, SKYBLUE);
		this->tiles.emplace_back(-1, 0, SKYBLUE);
		this->tiles.emplace_back( 1, 0, SKYBLUE);
		this->tiles.emplace_back( 2, 0, SKYBLUE);
	}
};

class sShape : public shape {
public:
	sShape() : shape() {
		this->tiles.emplace_back( 0, 0, LIME);
		this->tiles.emplace_back( 1, 0, LIME);
		this->tiles.emplace_back( 0, 1, LIME);
		this->tiles.emplace_back(-1, 1, LIME);
	}
};

class zShape : public shape {
public:
	zShape() : shape() {
		this->tiles.emplace_back( 0, 0, RED);
		this->tiles.emplace_back(-1, 0, RED);
		this->tiles.emplace_back( 0, 1, RED);
		this->tiles.emplace_back( 1, 1, RED);
	}
};

class lShape : public shape {
public:
	lShape() : shape() {
		this->tiles.emplace_back( 0, 0, ORANGE);
		this->tiles.emplace_back(-1, 0, ORANGE);
		this->tiles.emplace_back(-2, 0, ORANGE);
		this->tiles.emplace_back( 0,-1, ORANGE);
	}
};

class jShape : public shape {
public:
	jShape() : shape() {
		this->tiles.emplace_back( 0, 0, BLUE);
		this->tiles.emplace_back( 0,-1, BLUE);
		this->tiles.emplace_back( 1, 0, BLUE);
		this->tiles.emplace_back( 2, 0, BLUE);
	}
};

template<typename T>
shape *createPiece() {
	return new T();
}

typedef shape *(*CreatePieceFn)();
const CreatePieceFn createFunctions[] = {
	[T_SHAPE] = &createPiece<tShape>,
	[O_SHAPE] = &createPiece<oShape>,
	[I_SHAPE] = &createPiece<iShape>,
	[S_SHAPE] = &createPiece<sShape>,
	[Z_SHAPE] = &createPiece<zShape>,
	[L_SHAPE] = &createPiece<lShape>,
	[J_SHAPE] = &createPiece<jShape>
};
static_assert((sizeof(createFunctions) / sizeof(*createFunctions)) == N_SHAPES, "Number of create piece functions does not match number of shapes in enum");

shape *randomPiece() {
	static random_device rd;
	static mt19937 rng(rd());
	static uniform_int_distribution<int> uni(0, N_SHAPES-1);

	return createFunctions[uni(rng)]();
}

int main() {

	const float DROPTIME = 0.75;
	float dropTime = DROPTIME; // time to drop one space

	shape *falling = nullptr;
	falling = randomPiece();
	optional<tile> history[gWidth][gHeight];

	InitWindow(gWidth * cellSize, gHeight * cellSize, "Jewel Dropper");

	SetTargetFPS(60);
	while (!WindowShouldClose()) {

		BeginDrawing();
		ClearBackground(BLACK);

		// update

		if (IsKeyPressed(KEY_LEFT))
			falling->shift(LEFT, history);
		if (IsKeyPressed(KEY_RIGHT))
			falling->shift(RIGHT, history);
		if (IsKeyPressed(KEY_UP))
			falling->pivot();

		dropTime -= GetFrameTime();
		if (dropTime <= 0 || IsKeyPressed(KEY_DOWN)) {
			dropTime = DROPTIME;
			// drop tiles
			if (falling->step(history)) {
				for (int i = 0; i < falling->tiles.size(); i++) {
					int tx = falling->x + falling->tiles[i].ox;
					int ty = falling->y + falling->tiles[i].oy;
					history[tx][ty] = falling->tiles[i];
				}
				delete falling;
				falling = randomPiece();
			}
		}

		// draw
		for (int y = 0; y < gHeight; y++) {
			for (int x = 0; x < gWidth; x++) {
				DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, WHITE);
				if (history[x][y].has_value()) {
					history[x][y].value().rawDraw(x, y);
				}
			}
		}

		falling->draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}