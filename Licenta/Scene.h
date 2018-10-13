#pragma once

#include "GL\glew.h"
#include "GL\freeglut.h"
#include "glm.hpp"
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
		static std::set<Scene *> scenes;

		std::map<const Model *, ModelInstanceData *> instances;
		std::vector<Water *> waterBodies;
		Lights lights;
		Skybox *skybox;
		Camera *camera;

		class Drawer
		{
			//const Scene& scene;
			Renderer renderer;
			Shader* activeShader;
			Camera& camera;
			const Lights& lights;

			SceneRenderingShader sceneShader;
			ShadowMapShader shadowMapShader;
			SkyboxShader skyboxShader;
			WaterShader waterShader;
			DirectShader directShader;

		public:
			Drawer(Camera& camera, const Lights& lights);
			inline void setActiveShader(Shader& shader) { glUseProgram(shader.getHandle());  activeShader = &shader; };
			void drawWater(const decltype(instances) reflectedObjects, const Water& water);
			void drawSkybox(const Skybox& skybox);
			void drawSkybox(const Skybox& skybox, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix);
			void drawSceneObjects(const decltype(instances) instances, const Framebuffer* framebuffer = nullptr);
			/** X and Y are the normalized screen coordinates (-1, 1) of the upper left corner. */
			void drawTextureOnScreen(const Texture* texture, float x, float y, float width, float height);
			void setAmbientLight(const AmbientLight& ambientLight);
			void setDirectionalLight(const DirectionalLight& directionalLight);
			void addPointLight(const PointLight& pointLight);
			void addSpotLight(const SpotLight& spotLight);
			void setAmbientLightColor(const glm::vec3& color);
			void setAmbientLightIntensity(float intensity);
			void setPointLightPosition(const glm::vec3& position);
			void setSpotLightPosition(const glm::vec3& position);
		};
		Drawer drawer;


	public:
		Scene();
		~Scene();
		void drawAll();
		Object* addObject(Object *ob);
		void removeObject(Object *ob);
		void addWaterBody(float height, float northBorder, float southBorder, float eastBorder, float westBorder);
		void setAmbientLight(const glm::vec3& color, float intensity);
		DirectionalLight* setDirectionalLight(const glm::vec3& direction, float diffuseIntensity, const glm::vec3& diffuseColor = glm::vec3(1), const glm::vec3& specularColor = glm::vec3(1));
		PointLight* addPointLight(const glm::vec3& position, float power, float quadraticFactor, float linearFactor, float constantFactor = 1.f, const glm::vec3& diffuseColor = glm::vec3(1), const glm::vec3& specularColor = glm::vec3(1));
		SpotLight* addSpotLight(const glm::vec3& position, const glm::vec3& direction, float innerAngleInDegrees, float outerAngleInDegrees, float power, float quadraticFactor, float linearFactor, float constantFactor = 1.f, const glm::vec3& diffuseColor = glm::vec3(1), const glm::vec3& specularColor = glm::vec3(1));
		glm::vec3 getAmbientLightColor();
		float getAmbientLightIntensity();
		const DirectionalLight& getDirectionalLights() const	{ return *lights.directionalLight; }
		const std::set<PointLight *>& getPointLights() const	{ return lights.pointLights; }
		const std::set<SpotLight *>& getSpotLights() const		{ return lights.spotLights; }
		void setAmbientLightColor(const glm::vec3& color);
		void setAmbientLightIntensity(float intensity);
		void setPointLightPosition(const glm::vec3& position);
		void setSpotLightPosition(const glm::vec3& position);
		Camera* getCamera()		{ return camera; }
		void setSkybox(Skybox *skybox)
		{
			Scene::skybox = skybox;
		}
	};
}
