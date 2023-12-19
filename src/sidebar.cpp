
#include "sidebar.hpp"
#include "mergeify.hpp"

const float foreheadH = cellSize * 4;
const int gap = 5;
const float roundness = 0.1f;

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

void drawGUIPanel() {
	DrawOutlineRectangle(buttonQuad, 3, GRAY, DARKGRAY);
	// DrawRoundedOuaaatlineRect(buttonQuad, 3, roundness, GRAY, BLACK);
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
	int tw = MeasureText(queue, 30);
	ShadowText(queue, futureQuad.x + futureQuad.width/2 - tw/2, futureQuad.y + 10, 30, BLUE);
	int mGap = 10;
	DrawOutlineRectangle(futureQuad.x + mGap, futureQuad.y + 35, futureQuad.width - mGap*2, futureQuad.height - 30 - mGap, 4, BLACK, DARKGRAY);

	for (int i = 0; i < 3; i++) { // go through 3 queue places
		int cy = futureQuad.y + (i + 1) * 80;
		int ox = g.currentPiece[i + 1]->midx;
		int oy = g.currentPiece[i + 1]->midy;
		for (tile t : g.currentPiece[i + 1]->tiles) { // tiles
			DrawOutlineRectangle(
				cx + ox * cellSize + t.ox * cellSize,
				cy + oy * cellSize + t.oy * cellSize + 15,
				cellSize, cellSize, 
				3, t.color, BLACK
			);
		}
	}
}


void drawSidebar(game& g) {
	BeginMode2D(sideBarTranslation);

	drawGUIPanel();
	drawScore(g);
	drawHold(g);
	drawQueue(g);

	EndMode2D();
}