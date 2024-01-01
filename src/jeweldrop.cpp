
#include "jeweldrop.hpp"
#include "shape.hpp"
#include "enums.hpp"

using namespace std;



Texture2D jewel;
Music music;
void loadJD() { 
	jewel = LoadTexture("assets/jewel.png"); 
	music = LoadMusicStream("assets/Twister_Tetris.mp3");
	PlayMusicStream(music);
	SetMusicVolume(music, 0.5);
}
void unloadJD() { 
	UnloadTexture(jewel); 
	UnloadMusicStream(music);
}
void updateJD() {
	UpdateMusicStream(music);
}


Game::Game() {
	for (int i = 0; i < 4; i++)
		currentPiece[i] = randomPiece(history);
	currentPiece[0]->gravity(history);
	score = 0;
	holding = nullptr;
	state = UNPAUSED;
	score = 0;
	tilesDropped = 0;
	tilesDroppedHolding = -1;

}

bool Game::isOccupied(int x, int y) {
	if (x < 0 || x >= gWidth || y >= gHeight) return true;
	return history[x][y].has_value();
}

bool Game::rowCompleted(int y) {
	for (int x = 0; x < gWidth; x++) {
		if (!isOccupied(x, y))
			return false;
	}
	return true;
}
bool Game::removeRow(int y) {
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
void Game::removeCompleted() {
	// use when wanting to reduce all complete rows
	state = UNPAUSED;
	for (int row : completedRows) {
		removeRow(row);
		completedRows.erase(row);
	}
	gravity();
}

// true if game continues, or false if gameover
bool Game::step() {
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
		if (rowCompleted(row)) {
			completedRows.insert(row);
			tRows++;
			state = REDUCINGROWS;
		}
	}
	score += scoreMap[tRows];

	delete currentPiece[0];
	for (int i = 1; i < 4; i++) // push shapes up in the queue
		currentPiece[i-1] = currentPiece[i];

	currentPiece[0]->gravity(history);
	currentPiece[3] = randomPiece(history);

	tilesDropped++;
	return true;
}

void Game::shift(shiftDir LR) {
	currentPiece[0]->shift(LR, history);
	currentPiece[0]->gravity(history);
}

void Game::pivot() {
	currentPiece[0]->pivot(history);
	currentPiece[0]->gravity(history);
}

void Game::holdSwap() {
	if (tilesDropped == tilesDroppedHolding) return; // disallow swapping to buy time
	tilesDroppedHolding = tilesDropped;
	Shape *cache = holding;
	holding = currentPiece[0];

	// ensure original rotation - prefers wider than taller
	holding->x =  4; // prevent collisions with walls
	holding->y = -3; // prevent collisions with history
	while (holding->rotation != 0) holding->pivot(history);

	currentPiece[0] = cache;
	if (!currentPiece[0])
		currentPiece[0] = randomPiece(history);

	currentPiece[0]->x = 4;
	currentPiece[0]->y = -2;
	currentPiece[0]->gravity(history);
}

const Tile& Game::tileAt(int x, int y) {
	// assumes that x, y is within bounds
	return this->history[x][y].value();
}

void Game::gravity() {
	currentPiece[0]->gravity(history);
}