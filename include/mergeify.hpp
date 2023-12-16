#pragma once

extern "C" {
#include <raylib.h>
}



void ShadowText(const char *text, int posX, int posY, int fontSize, Color forground, Color shadow);
void ShadowText(const char *text, int posX, int posY, int fontSize, Color forground);

void DrawOutlineRectangle(Rectangle rect, int thickness, Color color, Color outline);
void DrawOutlineRectangle(int posX, int posY, int width, int height, int thickness, Color color, Color outline);