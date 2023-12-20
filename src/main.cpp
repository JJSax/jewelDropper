
#include <raylib.h>
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>
#include <string>
// #include <cstring> // for strdup

#include "jeweldrop.hpp"
#include "sidebar.hpp"

/*
TODO add vfx to Slam
TODO add vfx to row removal
TODO use a/d + ←/→ for left right
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

using namespace std;

void dostep(game& g, bool *failScore) {
	if (g.step()) return;
	*failScore = true;
	g.state = GAMEOVER;
}

void update(game& g, bool *failScore) {
	if (IsKeyPressed(KEY_LEFT)) {
		g.shift(LEFT);
		holdShiftTime = HOLDSHIFTTIME * 4;
	}
	if (IsKeyPressed(KEY_RIGHT)) {
		g.shift(RIGHT);
		holdShiftTime = HOLDSHIFTTIME * 4;
	}
	bool l = IsKeyDown(KEY_LEFT), r = IsKeyDown(KEY_RIGHT);
	if (l || r) {
		holdShiftTime -= GetFrameTime();
		if (holdShiftTime <= 0) {
			holdShiftTime = HOLDSHIFTTIME;
			if (!(l && r))
				g.shift(l ? LEFT : RIGHT);
		}
	}
	if (IsKeyPressed(KEY_UP))
		g.pivot();
	if (IsKeyPressed(KEY_DOWN)) {
		dostep(g, failScore);
		holdDropTime = HOLDDROPTIME * 4;
	}
	if (IsKeyDown(KEY_DOWN)) {
		holdDropTime -= GetFrameTime();
		if (holdDropTime <= 0) {
			g.score++;
			holdDropTime = HOLDDROPTIME;
			forceDrop = true;
		}
	}

	if (IsKeyPressed(KEY_SPACE)) {
		g.score += (g.currentPiece[0]->settledY - g.currentPiece[0]->y) * 3;
		g.currentPiece[0]->y = g.currentPiece[0]->settledY;
		dostep(g, failScore);
	}

	dropTime -= GetFrameTime();
	if (dropTime <= 0 || forceDrop) {
		dropTime = DROPTIME;
		forceDrop = false;
		dostep(g, failScore);
	}

	if (IsKeyPressed(KEY_C)) {
		g.holdSwap();
	}
}

int main(void) {

	game g;
	bool failScore = false;
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
		if (!failScore && g.state == UNPAUSED) {
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

		if (failScore) {
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