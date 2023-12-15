extern "C" {
#include <raylib.h>
}
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>
#include <string>

#include "jeweldrop.hpp"

/*
*/

const float DROPTIME = 0.75;
float dropTime = DROPTIME; // time to drop one space

using namespace std;

void update(game& g, bool *failScore) {
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
			int sw = GetScreenWidth();
			int sh = GetScreenHeight();
			DrawRectangle(0, 0, sw, sh, Fade(RED, 0.2));
			int tw = MeasureText("DEFEAT", 40);
			DrawText("DEFEAT", sw/2 - tw/2, sh/2, 40, RED);

			const char *str = TextFormat("Score: %i", g.score);
			tw = MeasureText(str, 30);
			DrawText(TextFormat("Score: %i", g.score), sw/2 - tw/2, sh/2 + 40, 30, RED);
		}

		EndDrawing();
	}

	CloseWindow();
	return 0;
}