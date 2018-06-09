#pragma once

// Clasa in care e implementata logica programului
#include "GL\glew.h"
#include "GL\freeglut.h"
#include "Scene.h"

class ControllerManager
{
public:
	static void init();
	static void update();
};

struct InputManager
{
	int xMouseLast;
	int yMouseLast;
	bool upArrowPressed = false;
	bool downArrowPressed = false;
	bool leftArrowPressed = false;
	bool rightArrowPressed = false;
	bool rightShiftPressed = false;
	bool leftShiftPressed = false;
	bool rightCtrlPressed = false;
	bool leftCtrlPressed = false;
	bool mouseLeftButtonPressed = false;
	bool mouseMiddleButtonPressed = false;
	bool mouseRightButtonPressed = false;
	bool keyPressed[256];
};

void keyboardButtonPress(unsigned char, int, int);
void keyboardButtonRelease(unsigned char, int, int);
void keyboardSpecialButtonPress(int, int, int);
void keyboardSpecialButtonRelease(int, int, int);
void mouseButtonAction(int, int, int, int);
void mouseMotionAction(int, int);
