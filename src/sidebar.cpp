
#include "sidebar.hpp"
#include "options.hpp"

const int gap = 5;
const int font = 40;
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
Gamestate previousState = UNPAUSED;


bool testButton(Game& g, Gamestate previous, int measure, int x) {
	static int boardW = cellSize * gWidth;
	return (IsMouseButtonPressed(0)
	&& GetMouseX() > boardW + x && GetMouseX() < boardW + x + measure
	&& GetMouseY() > 15 && GetMouseY() < 15 + font);
}

void drawGUIPanel(Game& g) {
	DrawOutlineRectangle(buttonQuad, 3, GRAY, DARKGRAY);
	static int up = MeasureText("Unpause", font);
	static int ps = MeasureText(  "Pause", font);
	static int rp = MeasureText( "Replay", font);
	int x;
	const char* txt = "";

	Rectangle button = {
		buttonQuad.x + buttonQuad.width/2 - up/2 - 5,
		10.0f,
		up + 10.0f,
		font + 10.0f
	};

	DrawOutlineRectangle(button, 4, SLATEGRAY, DARKGRAY);
	switch (g.state) {
		case PAUSED:
			x = buttonQuad.x + buttonQuad.width/2 - up/2;
			if (testButton(g, previousState, up, x)) {
				g.state = previousState;
				previousState = PAUSED;
			}
			txt = "Unpause";
			break;
		case UNPAUSED:
			x = buttonQuad.x + buttonQuad.width/2 - ps/2;
			if (testButton(g, previousState, ps, x)) {
				previousState = g.state;
				g.state = PAUSED;
			}
			txt = "Pause";
			break;
		case REDUCINGROWS:
			x = buttonQuad.x + buttonQuad.width/2 - ps/2;
			if (testButton(g, previousState, ps, x)) {
				previousState = g.state;
				g.state = PAUSED;
			}
			txt = "Pause";
			break;
		case GAMEOVER:
			x = buttonQuad.x + buttonQuad.width/2 - rp/2;
			if (testButton(g, previousState, rp, x)) {
				g.state = UNPAUSED;
				previousState = UNPAUSED;
				g = Game();
			}
			txt = "Replay";
			break;
		default: // currently blank button when clearing row
			break;
	}
		
	ShadowText(txt, x, 18, font, SKYBLUE);
}

void drawScore(Game& g) {
	DrawOutlineRectangle(scoreQuad, 3, GRAY, DARKGRAY);
	const char *str = TextFormat("%i", g.score);
	static const char *scr = "Score";
	int tw = MeasureText(scr, 40);
	ShadowText(scr, sidebarWidth /2 - tw/2, foreheadH + 20, 40, BLUE);
	tw = MeasureText(str, 30);
	ShadowText(str, sidebarWidth/2 - tw/2, foreheadH + 60, 30, BLUE);
}

void drawHold(Game& g) {
	DrawOutlineRectangle(holdQuad, 3, GRAY, DARKGRAY);
	static const char *hold = "HOLD";
	int tw = MeasureText(hold, 30);
	ShadowText(hold, holdQuad.x + holdQuad.width/2 - tw/2, holdQuad.y + 7, 30, BLUE);
	int mGap = 10;
	DrawOutlineRectangle(holdQuad.x + mGap, holdQuad.y + 30, holdQuad.width - mGap*2, holdQuad.height - 20 - mGap, 4, BLACK, DARKGRAY);
	if (!g.holding) return;
	int cy = holdQuad.y + holdQuad.height/2 + 30;
	for (Tile t : g.holding->tiles) {
		DrawOutlineRectangle(
			cx + g.holding->midx * cellSize + t.ox * cellSize,
			cy + g.holding->midy * cellSize + t.oy * cellSize,
			cellSize, cellSize,
			3, t.color, BLACK
		);
	}
}

void drawQueue(Game& g) {
	static Rectangle futureQuad = {
		gap, holdQuad.y + scoreQuad.height,
		sidebarWidth - gap * 2, GetScreenHeight() - holdQuad.y - holdQuad.height - gap
	};
	DrawOutlineRectangle(futureQuad, 3, GRAY, DARKGRAY);
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
		for (Tile t : g.currentPiece[i]->tiles) { // tiles
			DrawOutlineRectangle(
				cx + ox * cellSize + t.ox * cellSize,
				cy + oy * cellSize + t.oy * cellSize,
				cellSize, cellSize,
				3, t.color, BLACK
			);
		}
	}
}


void drawSidebar(Game& g) {
	BeginMode2D(sideBarTranslation);

	drawGUIPanel(g);
	drawScore(g);
	drawHold(g);
	drawQueue(g);

	EndMode2D();
}