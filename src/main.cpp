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

const float DROPTIME = 0.75;
float dropTime = DROPTIME; // time to drop one space

using namespace std;

void update(game& g, bool *failScore) {

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
		if (!g.step()) *failScore = true;
	}
}

int main() {

	game g;
	bool failScore = false;

	InitWindow(gWidth * cellSize, gHeight * cellSize, "Jewel Dropper");

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		// update
		// if (IsKeyPressed(KEY_LEFT))
		// 	g.shift(LEFT);
		// if (IsKeyPressed(KEY_RIGHT))
		// 	g.shift(RIGHT);
		// if (IsKeyPressed(KEY_UP))
		// 	g.pivot();

		// dropTime -= GetFrameTime();
		// if (dropTime <= 0 || IsKeyPressed(KEY_DOWN)) {
		// 	dropTime = DROPTIME;
		// 	if (!g.step()) failScore = true;
		// }
		if (!failScore) {
			update(g, &failScore);
		}

		// draw
		BeginDrawing();
		ClearBackground(BLACK);
		for (int y = 0; y < gHeight; y++) {
			for (int x = 0; x < gWidth; x++) {
				DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, WHITE);
				if (g.isOccupied(x, y))
					g.tileAt(x, y).rawDraw(x, y);
			}
		}

		g.currentPiece->draw();

		if (failScore) {
			DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(RED, 0.2));
			int tw = MeasureText("DEFEAT", 40);
			DrawText("DEFEAT", GetScreenWidth()/2 - tw/2, GetScreenHeight()/2, 40, RED);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}