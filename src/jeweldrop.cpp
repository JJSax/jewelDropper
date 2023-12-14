#include "jeweldrop.hpp"
#include <vector>
#include <optional>
#include <random>

using namespace std;

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


shape::shape() {
	this->x = 4;
	this->y = 2;
	this->settled = false;
}

bool shape::step(optional<tile> history[][gHeight]) {
	y++;
	for (int i = 0; i < tiles.size(); i++)
	{
		int tx = x + tiles[i].ox;
		int ty = y + tiles[i].oy;
		if (ty >= gHeight || history[tx][ty].has_value())
		{
			y--;
			return true;
		}
	}
	return false;
}

void shape::shift(shiftDir LR, optional<tile> history[][gHeight]) {
	x += LR;
	for (int i = 0; i < tiles.size(); i++) {
		int tx = x + tiles[i].ox;
		int ty = y + tiles[i].oy;
		if (tx == gWidth || tx < 0 || history[tx][ty].has_value())
			x += -LR;
	}
}

void shape::pivot(optional<tile> history[][gHeight]) {
	//! CHECK THROUGH HISTORY
	for (int i = 1; i < tiles.size(); i++)
	{
		int c = tiles[i].ox;
		tiles[i].ox = -tiles[i].oy;
		tiles[i].oy =  c;
	}
}

void shape::draw() {
	for (int i = 0; i < tiles.size(); i++) {
		tiles[i].rawDraw(x + tiles[i].ox, y + tiles[i].oy);
	}
}

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
static_assert(
	(sizeof(createFunctions) / sizeof(*createFunctions)) == N_SHAPES,
	"Number of create piece functions does not match number of shapes in enum"
);

shape *randomPiece(void) {
	static random_device rd;
	static mt19937 rng(rd());
	static uniform_int_distribution<int> uni(0, N_SHAPES-1);

	return createFunctions[uni(rng)]();
}



game::game() {
	currentPiece = randomPiece();
}

bool game::isOccupied(int x, int y) {
	if (x < 0 || x >= gWidth || y >= gHeight) return true;
	return history[x][y].has_value();
}

void game::step() {
	if (!currentPiece->step(history)) return; // if cannot move down
	for (int i = 0; i < currentPiece->tiles.size(); i++) {
		int tx = currentPiece->x + currentPiece->tiles[i].ox;
		int ty = currentPiece->y + currentPiece->tiles[i].oy;
		history[tx][ty] = currentPiece->tiles[i];

	}
	delete currentPiece;
	currentPiece = randomPiece();
}

void game::shift(shiftDir LR) {
	currentPiece->shift(LR, this->history);
}

void game::pivot() {
	currentPiece->pivot(this->history);
}

const tile& game::tileAt(int x, int y) {
	// assumes that x, y is within bounds
	return this->history[x][y].value();
}