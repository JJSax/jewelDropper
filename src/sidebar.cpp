
#include "sidebar.hpp"
#include "mergeify.hpp"

const float foreheadH = cellSize * 4;
Camera2D sideBarTranslation = {
	.offset = (Vector2) {gWidth * cellSize, 0}, 
	.target = (Vector2) {0, 0},
	.rotation = 0.0f,
	.zoom = 1.0f
};
Rectangle buttonQuad = {0, 0, sidebarWidth, cellSize * 4};
Rectangle scoreQuad = {0, buttonQuad.height, sidebarWidth, cellSize * 4};
Rectangle holdQuad = {0, scoreQuad.y + scoreQuad.height, sidebarWidth, cellSize * 4};
Rectangle futureQuad = {0, holdQuad.y + scoreQuad.height, sidebarWidth, GetScreenHeight() - holdQuad.y - holdQuad.height};

const int cx = sidebarWidth/2;

void drawGUIPanel() {
	DrawOutlineRectangle(buttonQuad, 3, GRAY, BLACK);
}

void drawScore(game& g) {
	DrawOutlineRectangle(scoreQuad, 3, GRAY, BLACK);
	const char *str = TextFormat("%i", g.score);
	static const char *scr = "Score";
	int tw = MeasureText(scr, 40);
	ShadowText(scr, sidebarWidth /2 - tw/2, foreheadH + 20, 40, BLUE);
	tw = MeasureText(str, 30);
	ShadowText(str, sidebarWidth/2 - tw/2, foreheadH + 60, 30, BLUE);
}

void drawHold(game& g) {
	DrawOutlineRectangle(holdQuad, 3, GRAY, BLACK);
	static const char *hold = "HOLD";
	int tw = MeasureText(hold, 30);
	ShadowText(hold, holdQuad.x + holdQuad.width/2 - tw/2, holdQuad.y + 7, 30, BLUE);
	if (!g.holding) return;
	int cy = holdQuad.y + holdQuad.height/2 + 30; 
	for (tile t : g.holding->tiles) {
		// DrawOutlineRectangle(
		//     holdQuad.x + holdQuad.width/2 + t.ox * cellSize, 
		//     holdQuad.y + 30 + t.oy * cellSize, 
		//     cellSize, cellSize, 
		//     3, t.color, BLACK
		// );
		DrawOutlineRectangle(
			cx + g.holding->midx * cellSize + t.ox * cellSize,
			cy + g.holding->midy * cellSize + t.oy * cellSize,
			cellSize, cellSize, 
			3, t.color, BLACK
		);
	}
}

void drawQueue(game& g) {
	DrawOutlineRectangle(futureQuad, 3, GRAY, BLACK);
	static const char *queue = "Queue";
	int tw = MeasureText(queue, 30);
	ShadowText(queue, futureQuad.x + futureQuad.width/2 - tw/2, futureQuad.y + 10, 30, BLUE);

	for (int i = 0; i < 3; i++) { // go through 3 queue places
		for (tile t : g.currentPiece[i + 1]->tiles) { // tiles
			DrawOutlineRectangle(
				futureQuad.x + futureQuad.width/2 + t.ox * cellSize, 
				futureQuad.y + 50 + i * (cellSize * 3) + t.oy * cellSize, 
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