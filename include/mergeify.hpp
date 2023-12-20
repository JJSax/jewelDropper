#pragma once

#include <raylib.h>



void ShadowText(const char *text, int posX, int posY, int fontSize, Color forground, Color shadow);
void ShadowText(const char *text, int posX, int posY, int fontSize, Color forground);
// void ShadowText(const char *text, int poxX, int poxY, int fontSize, Color foreground, Color shadow=BLACK);

void DrawOutlineRectangle(Rectangle rect, int thickness, Color color, Color outline);
void DrawOutlineRectangle(int posX, int posY, int width, int height, int thickness, Color color, Color outline);

void DrawRoundedOutlineRect(
	Rectangle rect, float thickness, float roundness,
	Color color, Color outline
);
void DrawRoundedOutlineRect(
	int posX, int posY, int width, int height,
	float thickness, float roundness, Color color, Color outline
);