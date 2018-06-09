#pragma once

// Wrapper peste API-ul OpenGL
#include "GL\glew.h"
#include "GL\freeglut.h"
#include <iostream>
#include <fstream>
#include <vector>
#include "glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\quaternion.hpp"
#include "Utils.h"
#include "Camera.h"
#include "Model.h"
#include "Scene.h"
#include "Framebuffer.h"

namespace OpenGL
{
	// -----------------------------------------------------------------------
	// Constante, variabile si obiecte globale
	// -----------------------------------------------------------------------
	const int WINDOW_WIDTH = 1000;
	const int WINDOW_HEIGHT = 600;
	extern Scene* activeScene;

	void initializari();
	void render();
	void windowReshape(int width, int height);


	// -----------------------------------------------------------------------
	// -----------------------------------------------------------------------
}
