
#include <raylib.h>
#include <vector>
#include <iostream>

#include "jeweldrop.hpp"
#include "sidebar.hpp"
#include "mergeify.hpp"
#include "keybinds.hpp"
#include "options.hpp"

/*
IDEA occasional 'powerup' empty points in history that do things
	perhaps some can be good or bad, like hitting it forces a slam, but is worth a lot more points
*/

using namespace std;

vector<Rectangle> slams;
vector<float> slamAlpha;

void dostep(Game& g) {
	if (g.step()) return;
	g.state = GAMEOVER;
}

void update(Game& g) {
	if (g.state == REDUCINGROWS) { // if row animation is playing
		reduceTime -= GetFrameTime();
		blinkTime -= GetFrameTime() * 4;
		if (reduceTime > 0) return; // still reducing
		reduceTime.reset();
		blinkTime.reset();
		g.removeCompleted();
		return;
	}

	if (keyMapper.IsKeybindPressed(Keybinds::SHIFTLEFT)) {
		g.shift(LEFT);
		holdShiftTime.multReset(4);
	}
	if (keyMapper.IsKeybindPressed(Keybinds::SHIFTRIGHT)) {
		g.shift(RIGHT);
		holdShiftTime.multReset(4);
	}
	bool l = keyMapper.IsKeybindDown(Keybinds::SHIFTLEFT);
	bool r = keyMapper.IsKeybindDown(Keybinds::SHIFTRIGHT);
	if (l || r) {
		holdShiftTime -= GetFrameTime();
		if (holdShiftTime <= 0) {
			holdShiftTime.reset();
			if (!(l && r))
				g.shift(l ? LEFT : RIGHT);
		}
	}
	if (keyMapper.IsKeybindPressed(Keybinds::PIVOT))
		g.pivot();
	if (keyMapper.IsKeybindPressed(Keybinds::STEP)) {
		holdDropTime.multReset(4);
		dropTime.reset();
		dostep(g);
	}
	if (keyMapper.IsKeybindDown(Keybinds::STEP)) {
		holdDropTime -= GetFrameTime();
		if (holdDropTime <= 0) {
			g.score++;
			holdDropTime.reset();
			dropTime.reset();
			dostep(g);
		}
	}

	if (keyMapper.IsKeybindPressed(Keybinds::SLAM)) {
		float l = static_cast<float>(g.currentPiece[0]->getLeft() * cellSize);
		float r = static_cast<float>(g.currentPiece[0]->getRight() * cellSize);
		float y = static_cast<float>(g.currentPiece[0]->y * cellSize + cellSize);
		float h = static_cast<float>(g.currentPiece[0]->settledY * cellSize - g.currentPiece[0]->y * cellSize);
		Rectangle rect = {
			l + 5, y,
			r - l - 10,
			h
		};
		slams.emplace_back(rect);
		slamAlpha.emplace_back(1.0f);
		g.score += (g.currentPiece[0]->settledY - g.currentPiece[0]->y) * 3;
		g.currentPiece[0]->y = g.currentPiece[0]->settledY;
		dostep(g);
	}

	dropTime -= GetFrameTime();
	if (dropTime <= 0) {
		dropTime.reset();
		dostep(g);
	}

	if (keyMapper.IsKeybindPressed(Keybinds::SWAPHOLD)) {
		g.holdSwap();
	}
}


void drawTiles(Game& g) {
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

void drawGame(Game& g) {
	if (g.state == PAUSED) {
		DrawRectangle(0, -foreheadH, gWidth * cellSize, GetScreenHeight(), BLACK);
		static const int pd = MeasureText("PAUSED", 40);
		DrawText("PAUSED", gWidth * cellSize/2 - pd/2, GetScreenHeight() / 4, 40, WHITE);
		return;
	}

	// Draw Slam Animation
	for (int i = slams.size()-1; i >= 0; i--) { // reverse to avoid erase bug
		Rectangle r = slams[i];
		DrawRectangleGradientV(
			r.x, r.y, r.width, r.height, BLANK, Fade(GRAY, slamAlpha[i])
		);
		slamAlpha[i] -= GetFrameTime() * 2;
		if (slamAlpha[i] <= 0) {
			slams.erase(slams.begin() + i);
			slamAlpha.erase(slamAlpha.begin() + i);
		}
	}

	drawTiles(g);
}

int main(void) {

	Game g;
	Camera2D camera;
	camera.offset = (Vector2) {0, foreheadH};
	camera.target = (Vector2) {0, 0};
	camera.rotation = 0.0f;
	camera.zoom = 1.0f;

	InitWindow(gWidth * cellSize + sidebarWidth, gHeight * cellSize + foreheadH, "Jewel Dropper");

	InitAudioDevice();
	loadJD();
	loadOptions();
	SetTargetFPS(60);
	while (!WindowShouldClose()) {

		// update
		if (g.state != GAMEOVER && (g.state == UNPAUSED || g.state == REDUCINGROWS)) {
			update(g);
			updateJD();
		}

		// draw
		BeginDrawing();
		BeginMode2D(camera);
		ClearBackground(BLACK);
		drawGame(g);

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

	unloadJD();

	CloseWindow();
	return 0;
}