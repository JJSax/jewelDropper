
#include <optional>
#include <random>

#include "matris.hpp"
#include "shape.hpp"
#include "options.hpp"
#include "enums.hpp"

using namespace std;
enum classShapeTypes {
	T_SHAPE,
	O_SHAPE,
	I_SHAPE,
	S_SHAPE,
	Z_SHAPE,
	L_SHAPE,
	J_SHAPE,
	N_SHAPES
};

Shape::Shape(float midX, float midY): midx(midX), midy(midY) {
	x = 4;
	y = -2;
	settledY = -2;
}

// return true if can step down, false if not.
bool Shape::step(optional<Tile> history[][gHeight]) {
	y++;
	for (int i = 0; i < tiles.size(); i++)
	{
		int tx = x + tiles[i].ox;
		int ty = y + tiles[i].oy;
		if (ty >= gHeight || (ty >= 0 && history[tx][ty].has_value()))
		{ // when it settles
			y--;
			return false;
		}
	}
	return true;
}

void Shape::gravity(optional<Tile> history[][gHeight]) {
	// get where it will land if it falls straight down.
	settledY = -2;
	int cy = y;
	while (step(history))
		settledY++;
	y = cy;
	settledY += y + 2; // + 2 to account for starting point
}

bool Shape::shift(shiftDir LR, optional<Tile> history[][gHeight]) {
	x += LR;
	for (Tile t : tiles) {
		int tx = x + t.ox;
		int ty = y + t.oy;
		if (tx < 0 || tx == gWidth || (ty > 0 && history[tx][ty].has_value())) {
			x += -LR;
			return false;
		}
	}
	return true;
}

void Shape::pivot(optional<Tile> history[][gHeight]) {
	vector<pair<int, int>> newPos;
	for (int i = 1; i < tiles.size(); i++) {
		int newOX = -tiles[i].oy;
		int newOY = tiles[i].ox;
		int newX = x + newOX;
		int newY = y + newOY;
		if (newX < 0) {
			if (shift(RIGHT, history)) pivot(history);
			return;
		}
		else if (newX >= gWidth) {
			if (shift(LEFT, history)) pivot(history);
			return;
		}

		if (newY >= gHeight || (newY > 0 && history[newX][newY].has_value())) {
			// dont do anything with the new coords
			return;
		}
		newPos.emplace_back(newOX, newOY);
	}

	auto tIter = tiles.begin();
	tIter++; // skip origin block

	auto nIter = newPos.begin();
	while (tIter != tiles.end() && nIter != newPos.end()) {
		tIter->ox = nIter->first;
		tIter->oy = nIter->second;
		tIter++;
		nIter++;
	}
	rotation++;
	wrapNum(&rotation, 0, 4);
}

void Shape::draw() {
	for (int i = 0; i < tiles.size(); i++) {
		tiles[i].rawDraw(x + tiles[i].ox, y + tiles[i].oy);
	}
}

int Shape::getLeft() { //? consider caching this on creation/pivot/shift etc.
	short int tx = 5;
	for (Tile t: tiles) { if (t.ox < tx) tx = t.ox; }
	return x + tx;
}
int Shape::getRight() { //? consider caching this on creation/pivot/shift etc.
	short int tx = -5;
	for (Tile t: tiles) { if (t.ox > tx) tx = t.ox; }
	return x + tx + 1;
}



class tShape : public Shape {
public:
	tShape() : Shape(-0.5, -0.5) {
		tiles.emplace_back( 0, 0, PURPLE);
		tiles.emplace_back(-1, 0, PURPLE);
		tiles.emplace_back( 1, 0, PURPLE);
		tiles.emplace_back( 0,-1, PURPLE);
	}
};

class oShape : public Shape {
public:
	oShape() : Shape(-1, -1.5) {
		tiles.emplace_back( 0, 0, YELLOW);
		tiles.emplace_back( 1, 0, YELLOW);
		tiles.emplace_back( 0, 1, YELLOW);
		tiles.emplace_back( 1, 1, YELLOW);
	}
};

class iShape : public Shape {
public:
	iShape() : Shape(-1, -1) {
		tiles.emplace_back( 0, 0, SKYBLUE);
		tiles.emplace_back(-1, 0, SKYBLUE);
		tiles.emplace_back( 1, 0, SKYBLUE);
		tiles.emplace_back( 2, 0, SKYBLUE);
	}
};

class sShape : public Shape {
public:
	sShape() : Shape(-0.5, -1.5) {
		tiles.emplace_back( 0, 0, LIME);
		tiles.emplace_back( 1, 0, LIME);
		tiles.emplace_back( 0, 1, LIME);
		tiles.emplace_back(-1, 1, LIME);
	}
};

class zShape : public Shape {
public:
	zShape() : Shape(-0.5, -1.5) {
		tiles.emplace_back( 0, 0, RED);
		tiles.emplace_back(-1, 0, RED);
		tiles.emplace_back( 0, 1, RED);
		tiles.emplace_back( 1, 1, RED);
	}
};

class lShape : public Shape {
public:
	lShape() : Shape(0.5, -0.5) {
		tiles.emplace_back( 0, 0, ORANGE);
		tiles.emplace_back(-1, 0, ORANGE);
		tiles.emplace_back(-2, 0, ORANGE);
		tiles.emplace_back( 0,-1, ORANGE);
	}
};

class jShape : public Shape {
public:
	jShape() : Shape(-1.5, -0.5) {
		tiles.emplace_back( 0, 0, BLUE);
		tiles.emplace_back( 0,-1, BLUE);
		tiles.emplace_back( 1, 0, BLUE);
		tiles.emplace_back( 2, 0, BLUE);
	}
};

template<typename T>
Shape *createPiece() {
	return new T();
}

typedef Shape *(*CreatePieceFn)();
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

Shape *randomPiece(optional<Tile> history[][gHeight]) {
	static random_device rd;
	static mt19937 rng(rd());
	static uniform_int_distribution<int> uni(0, N_SHAPES-1);

	Shape *piece = createFunctions[uni(rng)]();

	return piece;
}