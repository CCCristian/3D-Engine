// Licenta.cpp : Grafica pe calculator
//
#include "pch.h"

#include "GL.h"
#include "OpenGL.h"
#include "ControllerManager.h"
#include "Debug.h"

constexpr char titluFereastra[] = "3D Rendering Engine";
constexpr long long targetFrateRate = 63;

void initializare()
{
	checkErrors();
	OpenGL::initializari();
	checkErrors();
	ControllerManager::init();
	checkErrors();
	TimeManager::init();
}

void update()
{
	static long long timeLeft;
	OpenGL::render();
	ControllerManager::update();
	timeLeft = 1000 / targetFrateRate - TimeManager::deltaMilliseconds();
	if (timeLeft > 0)
		std::this_thread::sleep_for(std::chrono::duration<float, std::milli>(timeLeft));
	glutSetWindowTitle((std::string(titluFereastra) + " - FPS = " + std::to_string(1000 / TimeManager::deltaMilliseconds())).c_str());
	TimeManager::update();
	glutPostRedisplay();
}

void close()
{

}

int main(int argc, char **argv)
{
	// Initializare OpenGL
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH);
	glutInitWindowSize(OpenGL::WINDOW_WIDTH, OpenGL::WINDOW_HEIGHT);
	glutInitWindowPosition((1366 - OpenGL::WINDOW_WIDTH) / 2, (768 - OpenGL::WINDOW_HEIGHT) / 4);
	glutCreateWindow(titluFereastra);
	if (glewInit() != GLEW_OK)
	{
		std::cerr << "Eroare la initializarea GLEW";
		std::cin.get();
		exit(1);
	}
	glGetError();		// Resets the error flag (bug in older GLEW versions)

	// General
	initializare();
	glutDisplayFunc(update);
	glutReshapeFunc(OpenGL::windowReshape);
	glutCloseFunc(close);

	// Input
	glutKeyboardFunc(keyboardButtonPress);
	glutKeyboardUpFunc(keyboardButtonRelease);
	glutSpecialFunc(keyboardSpecialButtonPress);
	glutSpecialUpFunc(keyboardSpecialButtonRelease);
	glutMouseFunc(mouseButtonAction);
	glutMotionFunc(mouseMotionAction);

	glutMainLoop();
}
