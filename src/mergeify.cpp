

#include "mergeify.hpp"

Color defaultShadow = BLACK;
int tOff = 3;

void ShadowText(const char *text, int posX, int posY, int fontSize, Color forground, Color shadow) {
	// const char *txt = text;
	DrawText(text, posX - tOff, posY - tOff, fontSize, shadow);
	DrawText(text, posX, posY, fontSize, forground);
}
void ShadowText(const char *text, int posX, int posY, int fontSize, Color forground) {
	ShadowText(text, posX, posY, fontSize, forground, defaultShadow);
}

void DrawOutlineRectangle(Rectangle rect, int thickness, Color color, Color outline) {
	DrawRectangleRec(rect, color);
	DrawRectangleLinesEx(rect, thickness, outline);
}
void DrawOutlineRectangle(int posX, int posY, int width, int height, int thickness, Color color, Color outline) {
	Rectangle rect = {
		static_cast<float>(posX),
		static_cast<float>(posY),
		static_cast<float>(width),
		static_cast<float>(height)
	};
	DrawOutlineRectangle(rect, thickness, color, outline);
}

void DrawRoundedOutlineRect(
	Rectangle rect, float thickness, float roundness, 
	Color color, Color outline
) {
	DrawRectangleRounded(rect, roundness, 0, color);
	DrawRectangleRoundedLines(rect, roundness, 0, thickness, color);
}
void DrawRoundedOutlineRect(
	int posX, int posY, int width, int height, 
	float thickness, float roundness, Color color, Color outline
) {
	Rectangle rect = {
		static_cast<float>(posX),
		static_cast<float>(posY),
		static_cast<float>(width),
		static_cast<float>(height)
	};
	DrawRoundedOutlineRect(rect, thickness, roundness, color, outline);
}