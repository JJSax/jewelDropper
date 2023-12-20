
#include "sidebar.hpp"
#include "mergeify.hpp"

const float foreheadH = cellSize * 4;
const int gap = 5;
const float roundness = 0.1f;
const Color SLATEGRAY {90, 118, 154, 255};
const Color DARKSLATEGRAY = {60, 88, 104, 255};

Camera2D sideBarTranslation = {
	.offset = (Vector2) {gWidth * cellSize, 0},
	.target = (Vector2) {0, 0},
	.rotation = 0.0f,
	.zoom = 1.0f
};
Rectangle buttonQuad = 	{gap, gap, sidebarWidth - gap * 2, cellSize * 4 - gap * 2};
Rectangle scoreQuad = 	{gap, buttonQuad.height + gap, sidebarWidth - gap * 2, cellSize * 4 - gap*2};
Rectangle holdQuad = 	{gap, scoreQuad.y + scoreQuad.height, sidebarWidth - gap * 2, cellSize * 4 - gap*2};

const int cx = sidebarWidth/2;

void drawGUIPanel(game& g) {
	DrawOutlineRectangle(buttonQuad, 3, GRAY, DARKGRAY);
	// DrawRoundedOuaaatlineRect(buttonQuad, 3, roundness, GRAY, BLACK);
	static int boardW = cellSize * gWidth;
	static int font = 40;
	int up = MeasureText("Unpause", font);
	int ps = MeasureText(  "Pause", font);
	int rp = MeasureText( "Replay", font);
	int x;

	Rectangle button = {
		buttonQuad.x + buttonQuad.width/2 - up/2 - 5,
		10.0f,
		up + 10.0f,
		font + 10.0f
	};

	// Color hoverCol = GetMouseX() > boardW + x && GetMouseX() < boardW + x + ps
	// 	&& GetMouseY() > 15 && GetMouseY() < 15 + font ? DARKSLATEGRAY : SLATEGRAY;
	DrawOutlineRectangle(button, 4, SLATEGRAY, DARKGRAY);

	switch (g.state) {
	case PAUSED:
		x = buttonQuad.x + buttonQuad.width/2 - up/2;
		ShadowText("Unpause", x, 18, font, SKYBLUE);
		if (IsMouseButtonPressed(0)
		&& GetMouseX() > boardW + x && GetMouseX() < boardW + x + up
		&& GetMouseY() > 15 && GetMouseY() < 15 + font)
			g.state = UNPAUSED;
		break;
	case UNPAUSED:
		x = buttonQuad.x + buttonQuad.width/2 - ps/2;
		ShadowText("Pause", x, 18, font, SKYBLUE);
		if (IsMouseButtonPressed(0)
		&& GetMouseX() > boardW + x && GetMouseX() < boardW + x + ps
		&& GetMouseY() > 15 && GetMouseY() < 15 + font)
			g.state = PAUSED;
		break;
	default: // GAMEOVER
		x = buttonQuad.x + buttonQuad.width/2 - rp/2;
		ShadowText("Replay", x, 18, font, SKYBLUE);
		if (IsMouseButtonPressed(0)
		&& GetMouseX() > boardW + x && GetMouseX() < boardW + x + rp
		&& GetMouseY() > 15 && GetMouseY() < 15 + font)
			g.state = UNPAUSED;
		break;
	}
}

void drawScore(game& g) {
	DrawOutlineRectangle(scoreQuad, 3, GRAY, DARKGRAY);
	// DrawRoundedOutlineRect(scoreQuad, 3, roundness, GRAY, BLACK);
	const char *str = TextFormat("%i", g.score);
	static const char *scr = "Score";
	int tw = MeasureText(scr, 40);
	ShadowText(scr, sidebarWidth /2 - tw/2, foreheadH + 20, 40, BLUE);
	tw = MeasureText(str, 30);
	ShadowText(str, sidebarWidth/2 - tw/2, foreheadH + 60, 30, BLUE);
}

void drawHold(game& g) {
	DrawOutlineRectangle(holdQuad, 3, GRAY, DARKGRAY);
	// DrawRoundedOutlineRect(holdQuad, 3, roundness, GRAY, BLACK);
	static const char *hold = "HOLD";
	int tw = MeasureText(hold, 30);
	ShadowText(hold, holdQuad.x + holdQuad.width/2 - tw/2, holdQuad.y + 7, 30, BLUE);
	int mGap = 10;
	DrawOutlineRectangle(holdQuad.x + mGap, holdQuad.y + 30, holdQuad.width - mGap*2, holdQuad.height - 20 - mGap, 4, BLACK, DARKGRAY);
	if (!g.holding) return;
	int cy = holdQuad.y + holdQuad.height/2 + 30;
	for (tile t : g.holding->tiles) {
		DrawOutlineRectangle(
			cx + g.holding->midx * cellSize + t.ox * cellSize,
			cy + g.holding->midy * cellSize + t.oy * cellSize,
			cellSize, cellSize,
			3, t.color, BLACK
		);
	}
}

void drawQueue(game& g) {
	static Rectangle futureQuad = {
		gap, holdQuad.y + scoreQuad.height,
		sidebarWidth - gap * 2, GetScreenHeight() - holdQuad.y - holdQuad.height - gap
	};
	DrawOutlineRectangle(futureQuad, 3, GRAY, DARKGRAY);
	// DrawRoundedOutlineRect(futureQuad, 3, roundness, GRAY, BLACK);
	static const char *queue = "Queue";
	static const int pieceGap = 80;
	int tw = MeasureText(queue, 30);
	ShadowText(queue, futureQuad.x + futureQuad.width/2 - tw/2, futureQuad.y + 10, 30, BLUE);
	int mGap = 10;
	DrawOutlineRectangle(futureQuad.x + mGap, futureQuad.y + 35, futureQuad.width - mGap*2, futureQuad.height - 30 - mGap, 4, BLACK, DARKGRAY);

	for (int i = 1; i < 4; i++) { // go through 3 queue places
		int cy = futureQuad.y + i * pieceGap + cellSize;
		float ox = g.currentPiece[i]->midx;
		float oy = g.currentPiece[i]->midy;
		for (tile t : g.currentPiece[i]->tiles) { // tiles
			DrawOutlineRectangle(
				cx + ox * cellSize + t.ox * cellSize,
				cy + oy * cellSize + t.oy * cellSize,
				cellSize, cellSize,
				3, t.color, BLACK
			);
		}
	}
}


void drawSidebar(game& g) {
	BeginMode2D(sideBarTranslation);

	drawGUIPanel(g);
	drawScore(g);
	drawHold(g);
	drawQueue(g);

	EndMode2D();
}