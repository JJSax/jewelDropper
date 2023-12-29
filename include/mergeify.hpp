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



class VariableGroup {
public:
	VariableGroup(float initialValue) : constant(initialValue), variable(initialValue) {}

	void  reset() { variable = constant; }
	float getValue() const { return variable; }
	float getConst() const { return constant; }
	void  multReset(float m) { variable = constant * m; }
	operator float() const { return variable; }
	void operator -= (float a) { variable -= a; }
	void operator *= (float a) { variable *= a; }

private:
	const float constant;
	float variable;
};

