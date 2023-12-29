
#include <raylib.h>
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>
#include <string>
#include <cmath>
// #include <cstring> // for strdup

#include "jeweldrop.hpp"
#include "sidebar.hpp"

/*
TODO add vfx to Slam
TODO add vfx to row removal
TODO shift piece when pivoting if possible (when touching edge)
  -- Avoid awkward moments wondering why it's not pivoting
IDEA occasional 'powerup' empty points in history that do things
	perhaps some can be good or bad, like hitting it forces a slam, but is worth a lot more points
*/

const float DROPTIME = 0.75;
float dropTime = DROPTIME; // time to drop one space
const float HOLDDROPTIME = 0.06;
float holdDropTime = HOLDDROPTIME;
bool forceDrop = false;
const float HOLDSHIFTTIME = 0.05;
float holdShiftTime = HOLDSHIFTTIME;
const float REDUCETIME = 1.0f;
float reduceTime = REDUCETIME;
const float BLINKTIME = 4.0f; // will be reduced by dt * 4.  two on/off cycle per second
float blinkTime = BLINKTIME;

using namespace std;

void dostep(game& g) {
	if (g.step()) return;
	g.state = GAMEOVER;
}

void update(game& g) {
	if (g.reducingRows) { // if row animation is playing
		reduceTime -= GetFrameTime();
		blinkTime -= GetFrameTime() * 4;
		if (reduceTime > 0) return; // still reducing
		reduceTime = REDUCETIME;
		g.reducingRows = false;
		blinkTime = BLINKTIME;
		for (int row : g.completedRows) {
			g.removeRow(row);
			g.completedRows.erase(row);
		}
		g.gravity();
		return;
	}; 

	if (IsKeyPressed(KEY_LEFT) || IsKeyPressed(KEY_A)) {
		g.shift(LEFT);
		holdShiftTime = HOLDSHIFTTIME * 4;
	}
	if (IsKeyPressed(KEY_RIGHT) || IsKeyPressed(KEY_D)) {
		g.shift(RIGHT);
		holdShiftTime = HOLDSHIFTTIME * 4;
	}
	bool l = IsKeyDown(KEY_LEFT) || IsKeyDown(KEY_A), r = IsKeyDown(KEY_RIGHT) || IsKeyDown(KEY_D);
	if (l || r) {
		holdShiftTime -= GetFrameTime();
		if (holdShiftTime <= 0) {
			holdShiftTime = HOLDSHIFTTIME;
			if (!(l && r))
				g.shift(l ? LEFT : RIGHT);
		}
	}
	if (IsKeyPressed(KEY_UP) || IsKeyPressed(KEY_W))
		g.pivot();
	if (IsKeyPressed(KEY_DOWN) || IsKeyPressed(KEY_S)) {
		holdDropTime = HOLDDROPTIME * 4;
		dropTime = DROPTIME;
		dostep(g);
	}
	if (IsKeyDown(KEY_DOWN) || IsKeyDown(KEY_S)) {
		holdDropTime -= GetFrameTime();
		if (holdDropTime <= 0) {
			g.score++;
			holdDropTime = HOLDDROPTIME;
			dropTime = DROPTIME;
			// forceDrop = true;
			dostep(g);
		}
	}

	if (IsKeyPressed(KEY_SPACE)) {
		g.score += (g.currentPiece[0]->settledY - g.currentPiece[0]->y) * 3;
		g.currentPiece[0]->y = g.currentPiece[0]->settledY;
		dostep(g);
	}

	dropTime -= GetFrameTime();
	if (dropTime <= 0 || forceDrop) {
		dropTime = DROPTIME;
		// forceDrop = false;
		dostep(g);
	}

	if (IsKeyPressed(KEY_C)) {
		g.holdSwap();
	}
}


void drawTiles(game& g) {
	for (int y = 0; y < gHeight; y++) {
		for (int x = 0; x < gWidth; x++) {
			DrawRectangleLines(x * cellSize, y * cellSize, cellSize, cellSize, WHITE);
			bool isCompleted = g.completedRows.count(y) == 1;
			if (g.isOccupied(x, y) && (!isCompleted || isCompleted && (int)blinkTime % 2 == 0))
				g.tileAt(x, y).rawDraw(x, y);
		}
	}
	for (auto tile : g.currentPiece[0]->tiles) {
		int px = g.currentPiece[0]->x + tile.ox;
		int py = g.currentPiece[0]->settledY + tile.oy;
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

	g.currentPiece[0]->draw();
}

int main(void) {

	game g;
	float foreheadH = cellSize * 4;
	Camera2D camera;
	camera.offset = (Vector2) {0, foreheadH};
	camera.target = (Vector2) {0, 0};
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	InitWindow(gWidth * cellSize + sidebarWidth, gHeight * cellSize + foreheadH, "Jewel Dropper");

	SetTargetFPS(60);
	while (!WindowShouldClose()) {


		// update
		if (g.state != GAMEOVER && g.state == UNPAUSED) {
			update(g);
		}

		// draw
		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(BLACK);
		if (g.state == PAUSED) {
			DrawRectangle(0, -foreheadH, gWidth * cellSize, GetScreenHeight(), BLACK);
			static const int pd = MeasureText("PAUSED", 40);
			DrawText("PAUSED", gWidth * cellSize/2 - pd/2, GetScreenHeight() / 4, 40, WHITE);
		}
		else {
			drawTiles(g);
		}

		if (g.state == GAMEOVER) {
			int sw = GetScreenWidth() - sidebarWidth;
			int sh = GetScreenHeight();
			DrawRectangle(0, 0, sw, sh, ColorBrightness(Fade(RED, 0.2), 0.7));
			const char *s = "GAME OVER";
			int tw = MeasureText(s, 40);
			DrawText(s, sw/2 - tw/2-3, sh/2-3, 40, BLACK);
			DrawText(s, sw/2 - tw/2, sh/2, 40, RED);

			const char *str = TextFormat("Score: %i", g.score);
			tw = MeasureText(str, 30);
			DrawText(str, sw/2 - tw/2-3, sh/2 + 40-3, 30, BLACK);
			DrawText(str, sw/2 - tw/2, sh/2 + 40, 30, RED);
		}
		EndMode2D();
		drawSidebar(g);
		EndDrawing();
	}

	CloseWindow();
	return 0;
}