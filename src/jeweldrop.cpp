#include "jeweldrop.hpp"
#include <vector>
#include <optional>
#include <random>
#include <set>

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
	this->y = -2;
	this->settled = false;
	this->settledY = -2;
}

// return true if can step down, false if not.
bool shape::step(optional<tile> history[][gHeight]) {
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

void shape::gravity(optional<tile> history[][gHeight]) {
	// get where it will land if it falls straight down.
	settledY = -2;
	int cy = y;
	while (step(history))
		settledY++;
	y = cy;
	settledY += y + 2; // + 2 to account for starting point
}

void shape::shift(shiftDir LR, optional<tile> history[][gHeight]) {
	x += LR;
	for (int i = 0; i < tiles.size(); i++) {
		int tx = x + tiles[i].ox;
		int ty = y + tiles[i].oy;
		if (tx < 0 || tx == gWidth || (ty > 0 && history[tx][ty].has_value()))
			x += -LR;
	}
}

void shape::pivot(optional<tile> history[][gHeight]) {
	vector<pair<int, int>> newPos;
	for (int i = 1; i < tiles.size(); i++) {
		int newOX = -tiles[i].oy;
		int newOY = tiles[i].ox;
		int newX = x + newOX;
		int newY = y + newOY;
		if (newX < 0 || newX >= gWidth || newY >= gHeight || (newY > 0 && history[newX][newY].has_value())) {
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

shape *randomPiece(optional<tile> history[][gHeight]) {
	static random_device rd;
	static mt19937 rng(rd());
	static uniform_int_distribution<int> uni(0, N_SHAPES-1);

	shape *piece = createFunctions[uni(rng)]();
	// piece->gravity(history);

	return piece;
}



game::game() {
	for (int i = 0; i < 4; i++)
	{
		currentPiece[i] = randomPiece(history);
		//! Too tired
		//! if I forget, my thought is to make this a queue/list of 4
		//![0] is currentPiece as used before
		//! think about if history shoudl be passed into randomPiece
		//!  since it only needs to be used on spawn. could use gravite right?
	}
	
	// currentPiece = randomPiece(history);
	currentPiece[0]->gravity(history);
	score = 0;
}

bool game::isOccupied(int x, int y) {
	if (x < 0 || x >= gWidth || y >= gHeight) return true;
	return history[x][y].has_value();
}

bool game::removeRowIfCompleted(int y) {
	for (int x = 0; x < gWidth; x++) {
		if (!isOccupied(x, y)) {
			return false;
		}
	}
	for (int i = y; i > 0; i--) {
		for (int x = 0; x < gWidth; x++) {
			history[x][i] = history[x][i-1];
		}
	}
	for (int x = 0; x < gWidth; x++) {
		history[x][0].reset();
	}
	return true;
}


bool game::step() {
	if (currentPiece[0]->step(history)) return true; // if can move down
	static const int scoreMap[5] = {
		0, 100, 400, 900, 2000
	};

	// check defeat
	for (auto& t : currentPiece[0]->tiles) {
		if (currentPiece[0]->y + t.oy < 0)
			return false;
	}

	set<int> rowsAffected;
	for (int i = 0; i < currentPiece[0]->tiles.size(); i++) {
		// settle pieces into history
		int tx = currentPiece[0]->x + currentPiece[0]->tiles[i].ox;
		int ty = currentPiece[0]->y + currentPiece[0]->tiles[i].oy;
		history[tx][ty] = currentPiece[0]->tiles[i];
		rowsAffected.insert(ty);
	}
	int tRows = 0;
	for (auto row : rowsAffected) {
		if (removeRowIfCompleted(row)) tRows++;
	}
	score += scoreMap[tRows];

	delete currentPiece[0];
	for (int i = 1; i < 4; i++)
		currentPiece[i-1] = currentPiece[i];

	currentPiece[0]->gravity(history);
	currentPiece[3] = randomPiece(history);

	tilesDropped++;
	return true;
}

void game::shift(shiftDir LR) {
	currentPiece[0]->shift(LR, history);
	currentPiece[0]->gravity(history);
}

void game::pivot() {
	currentPiece[0]->pivot(history);
	currentPiece[0]->gravity(history);
}

void game::holdSwap() {
	if (tilesDropped == tilesDroppedHolding) return; // disallow swapping to buy time
	tilesDroppedHolding = tilesDropped;
	isHolding = true;
	shape *cache = holding;
	holding = currentPiece[0];
	currentPiece[0] = cache;
	if (!currentPiece[0])
		currentPiece[0] = randomPiece(history);

	currentPiece[0]->x = 4;
	currentPiece[0]->y = -2;
	currentPiece[0]->gravity(history);
}

const tile& game::tileAt(int x, int y) {
	// assumes that x, y is within bounds
	return this->history[x][y].value();
}