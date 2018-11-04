// Clasa in care e implementata logica programului
#include "ControllerManager.h"
#include "OpenGL.h"
#include "Framebuffer.h"
#include "Object.h"
#include "Terrain.h"
#include "Debug.h"


InputManager Input;

OpenGL::Scene* scena;
OpenGL::Object* lumina;
OpenGL::Object* flashlight;
OpenGL::Object* copac;
bool useNormalMapping = true;

void ControllerManager::init()
{
	for (int i = 0; i < 256; i++)
		Input.keyPressed[i] = false;

	scena = new OpenGL::Scene();
	OpenGL::activeScene = scena;
	scena->getCamera()->translateBy(glm::vec3(0, -3, 15));
	scena->getCamera()->setFarZ(100000);
	scena->setSkybox(new OpenGL::Skybox("Resurse/Skyboxes/Skybox1/Right.tga", "Resurse/Skyboxes/Skybox1/Left.tga", "Resurse/Skyboxes/Skybox1/Front.tga", "Resurse/Skyboxes/Skybox1/Back.tga", "Resurse/Skyboxes/Skybox1/Up.tga", "Resurse/Skyboxes/Skybox1/Down.tga"));
	
	scena->setAmbientLight(glm::vec3(.6f, .8f, .8f), .15f);
	scena->setDirectionalLight(-glm::vec3(0.850026, 0.10232, 0.516707), 1.2f, glm::vec3((float)243/256, (float)228 / 256, (float)194 / 256), glm::vec3(1, 1, 1))->setPosition(glm::vec3(10, 2, 10));
	scena->addPointLight(glm::vec3(-0.5f, 0, 0), /*6.5f*/0, 1, 1, 1.f, glm::vec3(1, 1, 1), glm::vec3(1, 1, 1));
	scena->addSpotLight(glm::vec3(-5, 0, 2.7), glm::vec3(0, 0, -1), 15, 30, 11.8f, 0.2f, 0.4f, 1.f);
	//scena->setSpotLight(glm::vec3(0, 0, 1), glm::vec3(0, 0, -1), 15, 11.8f, 0.02f, 0.04f, 1.f);
	//scena->addSpotLight(glm::vec3(2, 2, 3), glm::vec3(0, 0, -1), 15, 30, 1.8f, 0.2f, 0.4f, 1.f);

	const OpenGL::Model *quad = OpenGL::Model::BaseModelGenerator::generateQuad("quad", "Resurse/Texturi/Ground.png", 1000);
	//const OpenGL::Model *teren = OpenGL::Terrain::Builder().addTexture("Resurse/Texturi/Grass.bmp").setTextureRepeatCount(50).setMaxHeight(2).setHeightMapFile("Resurse/Terenuri/DoubleBasin_big.png").build();
	const OpenGL::Model *teren = OpenGL::Terrain::Builder().addTexture("Resurse/Texturi/Ground.png").setTextureRepeatCount(50).setMaxHeight(30).setHeightMapFile("Resurse/Terenuri/Coast.png").build();
	//const OpenGL::Model *casa1 = OpenGL::Model::createModel("casa1", "Resurse/Modele/247_House 15_obj/247_House 15_obj.obj");
	//const OpenGL::Model *casa2 = OpenGL::Model::createModel("casa2", "Resurse/Modele/Farmhouse Maya 2016 Updated/farmhouse_obj.obj");
	const OpenGL::Model *tree = OpenGL::Model::createModel("tree", "Resurse/Modele/Tree/Tree poplar N151117.obj");
	//const OpenGL::Model *tree = OpenGL::Model::createModel("tree", "Resurse/Modele/MapleTree/MapleTree.obj");
	//const OpenGL::Model *tree = OpenGL::Model::createModel("tree", "Resurse/Modele/FirTree/fir.obj");
	const OpenGL::Model *dock = OpenGL::Model::createModel("dock", "Resurse/Modele/Dock/Dock.obj");

	//OpenGL::Object *house = scena->addObject(new OpenGL::Object(casa1));
	//house->setPosition(glm::vec3(-7, 3, 1.8));
	//house->setRotation(glm::vec3(glm::pi<float>()/2, 0.0f, 0.0f));
	//house->setScale(glm::vec3(0.01f, 0.01f, 0.01f));

	//house = scena->addObject(new OpenGL::Object(casa2));
	//house->setPosition(glm::vec3(-2, 3, 1.5));
	//house->setRotation(glm::vec3(glm::pi<float>() / 2, 0.0f, 0.0f));
	//house->setScale(glm::vec3(0.1f, 0.1f, 0.1f));

	OpenGL::Object *terenOb = &scena->addObject(new OpenGL::Object(teren))->setPosition(0, 20, -15).setScale(150, 150, 1);
	scena->addWaterBody(8.5, 1000, -1000, 1000, -1000);
	scena->addObject(new OpenGL::Object(dock))->setPosition(glm::vec3(0, 0, 9)).setScale(1);
	//scena->addObject(new OpenGL::Object(quad))->setScale(glm::vec3(1000, 1000, 1)).setPosition(glm::vec3(0, 0, -0.1));


	const float step = 7;
	const float scale = 0.0015f;
	glm::vec3 center(6, 30, 9);
	for (float i = -10; i <= 10; i += 2 * step)
		for (float j = -10; j <= 10; j += 2 * step)
		{
			scena->addObject(new OpenGL::Object(tree))->setPosition(glm::vec3(i + distr(random) * 3, j + distr(random) * 3, 0) + center).setScale(glm::vec3(scale, scale, scale)).setRotation(glm::vec3(glm::pi<float>() / 2, 0, 0));
		}


	//lumina = &scena->addObject(new OpenGL::Object(OpenGL::Model::BaseModelGenerator::generateCube("cub", OpenGL::Texture::getDefaultTexture())))->setScale(.2f);

	//const OpenGL::Model *weed = OpenGL::Model::createModel("weed", "Resurse/Modele/Weed3/grass-block.obj");
	//const float step = 0.05f;
	//for (float i = -1; i <= 1; i += 2 * step)
	//	for (float j = -1; j <= 1; j += 2 * step)
	//	{
	//		scena->addObject(new OpenGL::Object(weed))->setPosition(glm::vec3(30*i + distr(random)/5, 20*j + distr(random)/5, 0) + glm::vec3(0, 0, 0)).setScale(glm::vec3(1, 1, 1)).setRotation(glm::vec3(glm::pi<float>() / 2, 0, 0));
	//	}

	//scena->addObject(new OpenGL::Object(weed))->setPosition(glm::vec3(0, 0, 0)).setScale(glm::vec3(0.1, 0.1, 0.1)).setRotation(glm::vec3(glm::pi<float>() / 2, 0, 0));
}

void ControllerManager::update()
{
	const float viteza = 0.1f;

	// Miscarea camerei: taste sageti si left ctrl/shift
	if (Input.leftArrowPressed)
		OpenGL::activeScene->getCamera()->moveRight(-viteza);
	if (Input.rightArrowPressed)
		OpenGL::activeScene->getCamera()->moveRight(viteza);
	if (Input.upArrowPressed || Input.mouseLeftButtonPressed && Input.mouseRightButtonPressed)
		OpenGL::activeScene->getCamera()->moveForward(viteza);
	if (Input.downArrowPressed)
		OpenGL::activeScene->getCamera()->moveForward(-viteza);
	if (Input.rightShiftPressed)
		OpenGL::activeScene->getCamera()->moveUp(viteza);
	if (Input.rightCtrlPressed)
		OpenGL::activeScene->getCamera()->moveUp(-viteza);

	// Ajustare intensitate lumina ambientala: A / Q
	if (Input.keyPressed['q'])
		scena->setAmbientLightIntensity(scena->getAmbientLightIntensity() - viteza/3);
	if (Input.keyPressed['e'])
		scena->setAmbientLightIntensity(scena->getAmbientLightIntensity() + viteza/3);


	static float x = -0.5f;
	static glm::vec2 posCasa(2, 3);
	//static glm::vec3 pos = []
	//{
	//	glm::vec3 poss = (*(OpenGL::activeScene->getSpotLights().begin()))->getPosition();
	//	lumina->setPosition(poss);
	//	return poss;
	//}();
	//if (Input.keyPressed['p'])
	//{
	//	if (copac != nullptr)
	//	{
	//		OpenGL::activeScene->removeObject(copac);
	//		delete copac;
	//		copac = nullptr;
	//	}
	//	copac = nullptr;
	//}

	//if (Input.keyPressed['a'])
	//{
	//	pos.x -= viteza;
	//	lumina->setPosition(pos);
	//	scena->setSpotLightPosition(pos);
	//}
	//if (Input.keyPressed['d'])
	//{
	//	pos.x += viteza;
	//	lumina->setPosition(pos);
	//	scena->setSpotLightPosition(pos);
	//}
	//if (Input.keyPressed['w'])
	//{
	//	pos.y += viteza;
	//	lumina->setPosition(pos);
	//	scena->setSpotLightPosition(pos);
	//}
	//if (Input.keyPressed['s'])
	//{
	//	pos.y -= viteza;
	//	lumina->setPosition(pos);
	//	scena->setSpotLightPosition(pos);
	//}
	//if (Input.keyPressed['j'])
	//{
	//	posCasa.x -= viteza;
	//	if (copac != nullptr)
	//		copac->setPosition(glm::vec3(posCasa, 0));
	//}
	//if (Input.keyPressed['l'])
	//{
	//	posCasa.x += viteza;
	//	if (copac != nullptr)
	//		copac->setPosition(glm::vec3(posCasa, 0));
	//}
	//if (Input.keyPressed['k'])
	//{
	//	posCasa.y -= viteza;
	//	if (copac != nullptr)
	//		copac->setPosition(glm::vec3(posCasa, 0));
	//}
	//if (Input.keyPressed['i'])
	//{
	//	posCasa.y += viteza;
	//	if (copac != nullptr)
	//		copac->setPosition(glm::vec3(posCasa, 0));
	//}
	//if (Input.keyPressed['r'])
	//{
	//	pos.z += viteza;
	//	lumina->setPosition(pos);
	//	scena->setSpotLightPosition(pos);
	//}
	//if (Input.keyPressed['f'])
	//{
	//	pos.z -= viteza;
	//	lumina->setPosition(pos);
	//	scena->setSpotLightPosition(pos);
	//}
}

void keyboardButtonPress(unsigned char c, int x, int y)
{
	constexpr char diff = 'a' - 'A';

	if (c == 27)		// Butonul de Esc
		glutLeaveMainLoop();
	else if (c >= 'A' && c <= 'Z')
		c += diff;
	else if (c == 13)	// Butonul de Enter
	{
		glm::vec3 look = OpenGL::activeScene->getCamera()->getLookVector();
		std::cout << "Camera look vector: (" << look.x << ", " << look.y << ", " << look.z << ").\n";
	}
	Input.keyPressed[c] = true;


	//Switch normal mapping
	if (Input.keyPressed['n'])
		useNormalMapping = !useNormalMapping;
}

void keyboardButtonRelease(unsigned char c, int x, int y)
{
	constexpr char diff = 'a' - 'A';

	if (c >= 'A' && c <= 'Z')
		c += diff;
	Input.keyPressed[c] = false;
}

void keyboardSpecialButtonPress(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		Input.upArrowPressed = true;
		break;
	case GLUT_KEY_DOWN:
		Input.downArrowPressed = true;
		break;
	case GLUT_KEY_LEFT:
		Input.leftArrowPressed = true;
		break;
	case GLUT_KEY_RIGHT:
		Input.rightArrowPressed = true;
		break;
	case GLUT_KEY_CTRL_L:
		Input.leftCtrlPressed = true;
		break;
	case GLUT_KEY_SHIFT_L:
		Input.leftShiftPressed = true;
		break;
	case GLUT_KEY_CTRL_R:
		Input.rightCtrlPressed = true;
		break;
	case GLUT_KEY_SHIFT_R:
		Input.rightShiftPressed = true;
		break;
	}
}

void keyboardSpecialButtonRelease(int key, int x, int y)
{
	switch (key)
	{
	case GLUT_KEY_UP:
		Input.upArrowPressed = false;
		break;
	case GLUT_KEY_DOWN:
		Input.downArrowPressed = false;
		break;
	case GLUT_KEY_LEFT:
		Input.leftArrowPressed = false;
		break;
	case GLUT_KEY_RIGHT:
		Input.rightArrowPressed = false;
		break;
	case GLUT_KEY_CTRL_L:
		Input.leftCtrlPressed = false;
		break;
	case GLUT_KEY_SHIFT_L:
		Input.leftShiftPressed = false;
		break;
	case GLUT_KEY_CTRL_R:
		Input.rightCtrlPressed = false;
		break;
	case GLUT_KEY_SHIFT_R:
		Input.rightShiftPressed = false;
		break;
	}
}

void mouseButtonAction(int button, int state, int x, int y)
{
	switch (button)
	{
	case GLUT_LEFT_BUTTON:
		Input.mouseLeftButtonPressed = (state == GLUT_DOWN ? true : false);
		break;
	case GLUT_MIDDLE_BUTTON:
		Input.mouseMiddleButtonPressed = (state == GLUT_DOWN ? true : false);
		break;
	case GLUT_RIGHT_BUTTON:
		Input.mouseRightButtonPressed = (state == GLUT_DOWN ? true : false);
		if (state == GLUT_DOWN)
		{
			Input.xMouseLast = x;
			Input.yMouseLast = y;
			glutSetCursor(GLUT_CURSOR_NONE);
		}
		else
			glutSetCursor(GLUT_CURSOR_INHERIT);
		break;
	}
}

void mouseMotionAction(int x, int y)
{
	if (Input.mouseRightButtonPressed)
	{
		OpenGL::activeScene->getCamera()->mouseLookDelta(Input.xMouseLast - x, Input.yMouseLast - y);
		glutWarpPointer(Input.xMouseLast, Input.yMouseLast);
	}
}
