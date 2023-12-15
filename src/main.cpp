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
const float HOLDDROPTIME = 0.1;
float holdDropTime = HOLDDROPTIME;
bool forceDrop = false;

using namespace std;

void update(game& g, bool *failScore) {
	if (IsKeyPressed(KEY_LEFT))
		g.shift(LEFT);
	if (IsKeyPressed(KEY_RIGHT))
		g.shift(RIGHT);
	if (IsKeyPressed(KEY_UP))
		g.pivot();
	if (IsKeyDown(KEY_DOWN)) {
		holdDropTime -= GetFrameTime();
		if (holdDropTime <= 0) {
			g.score++;
			holdDropTime = HOLDDROPTIME;
			forceDrop = true;
		}
	}

	dropTime -= GetFrameTime();
	if (dropTime <= 0 || forceDrop) {
		dropTime = DROPTIME;
		forceDrop = false;
		if (!g.step()) *failScore = true;
	}
}

int main() {

	game g;
	bool failScore = false;
	float foreheadH = cellSize * 4;
	Camera2D camera;
	camera.offset = (Vector2) {0, foreheadH};
	camera.target = (Vector2) {0, 0};
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	InitWindow(gWidth * cellSize, gHeight * cellSize + foreheadH, "Jewel Dropper");

	SetTargetFPS(60);
	while (!WindowShouldClose()) {
		// update
		if (!failScore) {
			update(g, &failScore);
		}

		// draw
		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(BLACK);
		for (int y = 0; y < gHeight; y++) {
			for (int x = 0; x < gWidth; x++) {
				DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, WHITE);
				if (g.isOccupied(x, y))
					g.tileAt(x, y).rawDraw(x, y);
			}
		}
		for (auto tile : g.currentPiece->tiles) {
			int px = g.currentPiece->x + tile.ox;
			int py = g.currentPiece->settledY + tile.oy;
			Color c = tile.color;
			static int bd = 2; // border
			Rectangle r = {
				static_cast<float>(px * cellSize + bd), 
				static_cast<float>(py * cellSize + bd), 
				static_cast<float>(cellSize - bd*2), 
				static_cast<float>(cellSize - bd*2)
			};
			DrawRectangleLinesEx(r, 2, c);
			DrawRectangleRec(r, Fade(c, 0.2));
		}

		g.currentPiece->draw();

		if (failScore) {
			int sw = GetScreenWidth();
			int sh = GetScreenHeight();
			DrawRectangle(0, 0, sw, sh, ColorBrightness(Fade(RED, 0.2), 0.7));
			int tw = MeasureText("GAME OVER", 40);
			DrawText("GAME OVER", sw/2 - tw/2-3, sh/2-3, 40, BLACK);
			DrawText("GAME OVER", sw/2 - tw/2, sh/2, 40, RED);

			const char *str = TextFormat("Score: %i", g.score);
			tw = MeasureText(str, 30);
			DrawText(TextFormat("Score: %i", g.score), sw/2 - tw/2-3, sh/2 + 40-3, 30, BLACK);
			DrawText(TextFormat("Score: %i", g.score), sw/2 - tw/2, sh/2 + 40, 30, RED);
		}
		EndMode2D();
		EndDrawing();
	}

	CloseWindow();
	return 0;
}