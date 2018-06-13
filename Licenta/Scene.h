#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "glm.hpp"
#include <set>
#include "Model.h"
#include "Mesh.h"
#include "Object.h"
#include "Light.h"
#include "Camera.h"
#include "Shader.h"
#include "Skybox.h"
#include "Water.h"
#include "Renderer.h"

namespace OpenGL
{
	class Scene
	{
		struct AmbientLight
		{
		public:
			glm::vec3 color;
			float intensity;
		};
		static std::set<Scene *> scenes;

		SceneRenderingShader sceneShader;
		ShadowMapShader shadowMapShader;
		SkyboxShader skyboxShader;
		WaterShader waterShader;
		DirectShader directShader;

		Renderer renderer;
		Skybox *skybox;
		Camera *camera;

		std::map<const Model *, ModelInstanceData *> instances;
		std::vector<Water *> waterBodies;

		AmbientLight ambientLight;
		std::set<DirectionalLight *> directionalLights;
		std::set<PointLight *> pointLights;
		std::set<SpotLight *> spotLights;

		void drawSceneObjects(std::map<const Model *, ModelInstanceData *>& instances, bool drawForShadowMap);
		void drawTextureOnScreen(const Texture* texture, float x, float y, float width, float height);
		void uploadWorldMatrixToShader(const Shader& shader, const glm::mat4& world);
		void drawSkybox();

	public:
		Scene();
		~Scene();
		void drawAll();
		Object* addObject(Object *ob);
		void removeObject(Object *ob);
		void addWaterBody(float height, float northBorder, float southBorder, float eastBorder, float westBorder);
		void setAmbientLight(glm::vec3 color, float intensity);
		void addDirectionalLight(glm::vec3 direction, float diffuseIntensity, glm::vec3 diffuseColor = glm::vec3(1), glm::vec3 specularColor = glm::vec3(1));
		void addPointLight(glm::vec3 position, float power, float quadraticFactor, float linearFactor, float constantFactor = 1.f, glm::vec3 diffuseColor = glm::vec3(1), glm::vec3 specularColor = glm::vec3(1));
		void addSpotLight(glm::vec3 position, glm::vec3 direction, float innerAngleInDegrees, float outerAngleInDegrees, float power, float quadraticFactor, float linearFactor, float constantFactor = 1.f, glm::vec3 diffuseColor = glm::vec3(1), glm::vec3 specularColor = glm::vec3(1));
		glm::vec3 getAmbientLightColor();
		float getAmbientLightIntensity();
		const std::set<DirectionalLight *>& getDirectionalLights() const	{ return directionalLights; }
		const std::set<PointLight *>& getPointLights() const				{ return pointLights; }
		const std::set<SpotLight *>& getSpotLights() const					{ return spotLights; }
		void setAmbientLightColor(glm::vec3 color);
		void setAmbientLightIntensity(float intensity);
		void setPointLightPosition(glm::vec3 position);
		void setSpotLightPosition(glm::vec3 position);
		Camera* getCamera()		{ return camera; }
		void setSkybox(Skybox *skybox)
		{
			Scene::skybox = skybox;
		}
	};
}
