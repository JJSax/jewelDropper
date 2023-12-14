extern "C" {
#include <raylib.h>
}
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>

#include "jeweldrop.hpp"

/*
TODO decide if line completion detection is best in step or main loop or elsewhere
TODO check collision in pivot
*/

using namespace std;

int main() {

	const float DROPTIME = 0.75;
	float dropTime = DROPTIME; // time to drop one space
	game g;

	InitWindow(gWidth * cellSize, gHeight * cellSize, "Jewel Dropper");

	SetTargetFPS(60);
	while (!WindowShouldClose()) {

		BeginDrawing();
		ClearBackground(BLACK);

		// update

		if (IsKeyPressed(KEY_LEFT))
			g.shift(LEFT);
		if (IsKeyPressed(KEY_RIGHT))
			g.shift(RIGHT);
		if (IsKeyPressed(KEY_UP))
			g.pivot();

		dropTime -= GetFrameTime();
		if (dropTime <= 0 || IsKeyPressed(KEY_DOWN)) {
			dropTime = DROPTIME;
			g.step();
		}

		// draw
		for (int y = 0; y < gHeight; y++) {
			for (int x = 0; x < gWidth; x++) {
				DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, WHITE);
				if (g.isOccupied(x, y))
					g.tileAt(x, y).rawDraw(x, y);
			}
		}

		g.currentPiece->draw();

		EndDrawing();
	}

	CloseWindow();
	return 0;
}