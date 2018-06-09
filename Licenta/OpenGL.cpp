// Wrapper peste API-ul OpenGL
#include "OpenGL.h"

namespace OpenGL
{
	// -----------------------------------------------------------------------
	// Constante, variabile si obiecte globale/statice
	// -----------------------------------------------------------------------
	int currentWindowWidth = WINDOW_WIDTH;
	int currentWindowHeight = WINDOW_HEIGHT;
	Scene *activeScene;

	// ----------------------------------------------------------
	// Functii
	// ----------------------------------------------------------

	void initializari()
	{
		glClearColor(0.0, 0.0, 0.0, 1.0);
		Texture::init();
		glCullFace(GL_BACK);
		glEnable(GL_DEPTH_TEST);
		glEnable(GL_CULL_FACE);
		//glutFullScreen();
	}

	void render()
	{
		activeScene->drawAll();
		glFlush();
		glutSwapBuffers();
	}

	void windowReshape(int width, int height)
	{
		currentWindowWidth = width;
		currentWindowHeight = height;
		glViewport(0, 0, width, height);
		if (activeScene != nullptr)
			activeScene->getCamera()->setAspectRatio((float)width / height);
		//Framebuffer::resizeAllFramebuffersToCurrentWindow();
	}
}
