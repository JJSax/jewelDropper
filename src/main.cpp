extern "C" {
#include <raylib.h>
}
#include <cstdio>
#include <optional>
#include <vector>
#include <iostream>
#include <string>
// #include <cstring> // for strdup

#include "jeweldrop.hpp"
#include "mergeify.hpp"

/*
TODO next piece view
TODO make hold piece
TODO make hold piece view
TODO Score view
TODO add vfx to Slam
*/

const float DROPTIME = 0.75;
float dropTime = DROPTIME; // time to drop one space
const float HOLDDROPTIME = 0.06;
float holdDropTime = HOLDDROPTIME;
bool forceDrop = false;
const float HOLDSHIFTTIME = 0.05;
float holdShiftTime = HOLDSHIFTTIME;

const int sidebarWidth = cellSize * 7;

using namespace std;

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
		if (!g.step()) *failScore = true;
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
		g.currentPiece->y = g.currentPiece->settledY;
		if (!g.step()) *failScore = true;
	}

	dropTime -= GetFrameTime();
	if (dropTime <= 0 || forceDrop) {
		dropTime = DROPTIME;
		forceDrop = false;
		if (!g.step()) *failScore = true;
	}
}

void drawSidebar(game& g) {
	static const float foreheadH = cellSize * 4;
	static Camera2D sideBarTranslation = {
		.offset = (Vector2) {gWidth * cellSize, 0},
		.target = (Vector2) {0, 0},
		.rotation = 0.0f,
		.zoom = 1.0f
	};

	BeginMode2D(sideBarTranslation);

	Rectangle buttonQuad = {0, 0, sidebarWidth, cellSize * 4};
	DrawRectangleRec(buttonQuad, GRAY);
	DrawRectangleLinesEx(buttonQuad, 3, BLACK);

	Rectangle scoreQuad = {0, buttonQuad.height, sidebarWidth, cellSize * 4};
	DrawRectangleRec(scoreQuad, GRAY);
	DrawRectangleLinesEx(scoreQuad, 3, BLACK);
	const char *str = TextFormat("%i", g.score);
	const char *scr = "Score";
	int tw = MeasureText(scr, 40);
	ShadowText(scr, sidebarWidth /2 - tw/2, foreheadH + 20, 40, BLUE);
	tw = MeasureText(str, 30);

	ShadowText(str, sidebarWidth/2 - tw/2, foreheadH + 60, 30, BLUE);

	Rectangle holdQuad = {0, scoreQuad.y + scoreQuad.height, sidebarWidth, cellSize * 4};
	DrawRectangleRec(holdQuad, GRAY);
	DrawRectangleLinesEx(holdQuad, 3, BLACK);
	const char *hold = "HOLD";
	tw = MeasureText(hold, 30);
	ShadowText(hold, holdQuad.x + holdQuad.width/2 - tw/2, holdQuad.y + 4, 30, BLUE);

	Rectangle futureQuad = {0, holdQuad.y + scoreQuad.height, sidebarWidth, GetScreenHeight() - holdQuad.y - holdQuad.height};
	DrawRectangleRec(futureQuad, GRAY);
	DrawRectangleLinesEx(futureQuad, 3, BLACK);
	const char *queue = "Queue";
	tw = MeasureText(queue, 30);
	ShadowText(queue, futureQuad.x + futureQuad.width/2 - tw/2, futureQuad.y + 10, 30, BLUE);

	EndMode2D();
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