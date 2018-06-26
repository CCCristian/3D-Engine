#include "Scene.h"
#include "Error.h"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>

namespace OpenGL
{
	std::set<Scene *> Scene::scenes;

	Scene::Scene(): sceneShader("main.vert", "main.frag"),
				shadowMapShader("shadowMap.vert", "shadowMap.frag"),
				skyboxShader("skybox.vert", "skybox.frag"),
				waterShader("water.vert", "water.frag"),
				directShader("direct.vert", "direct.frag")
	{
		renderer.useShaderProgram(sceneShader);
		scenes.insert(this);

		camera = new Camera(16.f / 9.f, glm::pi<float>() / 3, 0.01f, 100.f);
		skybox = nullptr;

		// Init default ambient light
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.color, 1, 1, 1);
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.intensity, 0.f);

		// Init default directional light
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.intensity, 0.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.direction, 0, 0, -1);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.diffuseColor, 1, 1, 1);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.specularColor, 1, 1, 1);

		// Init default point light
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.position, 0, 0, 1);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.power, 0.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.quadraticFactor, 1.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.linearFactor, 1.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.constantFactor, 1.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.diffuseColor, 1, 1, 1);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.specularColor, 1, 1, 1);

		// Init default spot light
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.position, 0, 0, 1);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.direction, 0, 0, -1);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.innerAngleCosine, std::cos(15 * glm::pi<float>() / 180));
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.outerAngleCosine, std::cos(30 * glm::pi<float>() / 180));
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.power, 0.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.quadraticFactor, 1.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.linearFactor, 1.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.constantFactor, 1.f);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.diffuseColor, 1, 1, 1);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.specularColor, 1, 1, 1);
	}
	Scene::~Scene()
	{
		delete camera;
		scenes.erase(this);
	}
	void Scene::drawSceneObjects(const Framebuffer* framebuffer)
	{
		// Shadow maps rendering pass
		renderer.useShaderProgram(shadowMapShader);
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, directionalLight->getShadowFramebuffer()->getHandle());
		glUniformMatrix4fv(ShadowMapShader::uniformLocations.lightMatrix, 1, GL_FALSE, (const GLfloat*)directionalLight->getShadowMapMatrix());
		glClear(GL_DEPTH_BUFFER_BIT);
		for (auto it = instances.begin(), sf = instances.end(); it != sf; it++)
		{
			renderer.draw(*it->second, true);
		}
		checkErrors();


		// Objects rendering pass
		renderer.useShaderProgram(sceneShader);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, directionalLight->getShadowFramebuffer()->getHandle());
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.shadowSampler);
		glBindTexture(GL_TEXTURE_2D, directionalLight->getShadowFramebuffer()->getDepthTexture()->getHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer == nullptr ? 0 : framebuffer->getHandle());
		uploadWorldMatrixToShader(sceneShader, camera->getWorldMatrix());
		glm::vec3 position = camera->getPosition();
		sceneShader.loadUniform(sceneShader.uniformLocations.cameraPosition, position.x, position.y, position.z);
		glUniformMatrix4fv(sceneShader.uniformLocations.lightSpaceMatrix, 1, GL_FALSE, (const GLfloat *)directionalLight->getShadowMapMatrix());
		for (auto it = instances.begin(), sf = instances.end(); it != sf; it++)
		{
			renderer.draw(*it->second, false);
		}
		checkErrors();
	}

	/** X and Y are the normalized screen coordinates (-1, 1) of the upper left corner. */
	void Scene::drawTextureOnScreen(const Texture* texture, float x, float y, float width, float height)
	{
		static GLuint vao = Model::getQuadModel()->getMeshes()[0]->getHandle();

		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0 + DirectShader::uniformLocations.colorSampler);
		glBindTexture(GL_TEXTURE_2D, texture->getHandle());

		glm::mat4 transform = glm::translate(glm::mat4(1), glm::vec3(x, y - height, 0));
		transform = glm::scale(transform, glm::vec3(width / 2, height / 2, 0));
		transform = glm::translate(transform, glm::vec3(1, 1, 0));
		glUniformMatrix4fv(directShader.uniformLocations.transform, 1, GL_FALSE, reinterpret_cast<const GLfloat*>(&transform));

		checkErrors();
		glDepthMask(GL_FALSE);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		glDepthMask(GL_TRUE);
		checkErrors();
	}
	void Scene::uploadWorldMatrixToShader(const Shader& shader, const glm::mat4& world)
	{
		GLuint location = sceneShader.uniformLocations.world;
		if (shader.shaderType == Shader::ShaderType::ShadowMapShader)
			location = shadowMapShader.uniformLocations.lightMatrix;
		else if (shader.shaderType == Shader::ShaderType::WaterShader)
			location = waterShader.uniformLocations.world;
		else if (shader.shaderType == Shader::ShaderType::SkyboxShader)
			location = skyboxShader.uniformLocations.world;

		shader.loadUniform(location, world);
	}
	void Scene::drawSkybox()
	{
		if (skybox != nullptr)
		{
			glm::mat4 world = camera->getProjectionMatrix() * glm::mat4(glm::mat3(camera->getViewMatrix()));
			glDepthMask(GL_FALSE);
			renderer.draw(skybox, world);
			glDepthMask(GL_TRUE);
		}
	}
	void Scene::drawSkybox(const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
	{
		if (skybox != nullptr)
		{
			glm::mat4 world = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
			glDepthMask(GL_FALSE);
			renderer.draw(skybox, world);
			glDepthMask(GL_TRUE);
		}
	}
	void Scene::drawAll()
	{
		// Clear all framebuffers
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, directionalLight->getShadowFramebuffer()->getHandle());
		glClear(GL_DEPTH_BUFFER_BIT);
		for (Water* water : waterBodies)
		{
			water->getReflectionFramebuffer().useAndClear();
			water->getRefractionFramebuffer().useAndClear();
		}
		
		//static Framebuffer fa(Framebuffer::FramebufferType::Color);
		//static Framebuffer fb(Framebuffer::FramebufferType::Color);
		//fa.clear();  fb.clear();

		// Skybox rendering
		renderer.useShaderProgram(skyboxShader);
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		drawSkybox();
		camera->invertPitch();
		for (Water* water : waterBodies)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, water->getReflectionFramebuffer().getHandle());
			drawSkybox();
		}
		camera->invertPitch();
		for (Water* water : waterBodies)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, water->getRefractionFramebuffer().getHandle());
			drawSkybox();
		}

		drawSceneObjects();

		static float movement = 0.0f;
		movement = std::fmod((float)TimeManager::elapsedMilliseconds()/35000, 1.f);
		// Water rendering
		for (Water* water : waterBodies)
		{
			glm::vec3 initialCameraPosition = camera->getPosition();

			// Render reflection
			camera->setPosition(glm::vec3(initialCameraPosition.x, initialCameraPosition.y, 2*water->getHeight() - initialCameraPosition.z));
			camera->invertPitch();
			glm::vec3 position = camera->getPosition();
			sceneShader.loadUniform(sceneShader.uniformLocations.clipPlane, 0, 0, 1, -(water->getHeight()));
			sceneShader.loadUniform(sceneShader.uniformLocations.cameraPosition, position.x, position.y, position.z);
			uploadWorldMatrixToShader(sceneShader, camera->getWorldMatrix());
			water->getReflectionFramebuffer().bind();
			renderer.useShaderProgram(sceneShader);
			glEnable(GL_CLIP_DISTANCE0);
			drawSceneObjects(&water->getReflectionFramebuffer());
			glDisable(GL_CLIP_DISTANCE0);

			// Render refraction
			camera->setPosition(initialCameraPosition);
			camera->invertPitch();
			sceneShader.loadUniform(sceneShader.uniformLocations.clipPlane, 0, 0, -1, water->getHeight() + 0.1f);
			position = camera->getPosition();
			sceneShader.loadUniform(sceneShader.uniformLocations.cameraPosition, position.x, position.y, position.z);
			uploadWorldMatrixToShader(sceneShader, camera->getWorldMatrix());
			water->getRefractionFramebuffer().bind();
			renderer.useShaderProgram(sceneShader);
			glEnable(GL_CLIP_DISTANCE0);
			drawSceneObjects(&water->getRefractionFramebuffer());
			glDisable(GL_CLIP_DISTANCE0);

			// Combining reflection with refraction
			renderer.useShaderProgram(directShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);


			renderer.useShaderProgram(waterShader);
			uploadWorldMatrixToShader(waterShader, camera->getWorldMatrix());
			waterShader.loadUniform(waterShader.uniformLocations.cameraPosition, camera->getPosition());
			waterShader.loadUniform(waterShader.uniformLocations.directionalLight, directionalLight->getDirection());
			waterShader.loadUniform(waterShader.uniformLocations.directionalColor, directionalLight->getSpecularColor());
			waterShader.loadUniform(waterShader.uniformLocations.movement, movement);
			renderer.draw(*water);
			checkErrors();
		}

		renderer.useShaderProgram(directShader);
		//drawTextureOnScreen(shadowLight->getShadowFramebuffer()->getDepthTexture(), 0.1f, 0.9f, 0.8f, 0.8f);
		renderer.useShaderProgram(sceneShader);
		checkErrors();
	}
	Object* Scene::addObject(Object *ob)
	{
		//objects[ob->getModel()].insert(ob);
		auto it = instances.find(ob->getModel());
		if (it == instances.end())
			//instances.insert(std::pair<const Model *, ModelInstanceData *>(ob->getModel(), new ModelInstanceData(ob->getModel())));
			it = instances.insert(std::pair<const Model *, ModelInstanceData *>(ob->getModel(), new ModelInstanceData(ob->getModel()))).first;
		it->second->addInstance(ob);
		//instances[ob->getModel()]->addInstance(ob);
		return ob;
	}
	void Scene::removeObject(Object *ob)
	{
		//auto it = objects.find(ob->getModel());
		//if (it != objects.end())
		//{
		//	it->second.erase(ob);
		//	if (it->second.size() == 0)
		//		objects.erase(it);
		//}
		instances[ob->getModel()]->removeInstance(ob);
		if (instances[ob->getModel()]->getInstanceCount() == 0)
		{
			delete instances[ob->getModel()];
			instances.erase(ob->getModel());
		}
	}

	void Scene::addWaterBody(float height, float northBorder, float southBorder, float eastBorder, float westBorder)
	{
		waterBodies.push_back(new Water(height, northBorder, southBorder, eastBorder, westBorder));
	}

	void Scene::setAmbientLight(glm::vec3 color, float intensity)
	{
		if (intensity < 0)
			intensity = 0;
		ambientLight.color = color;
		ambientLight.intensity = intensity;

		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.color, ambientLight.color.r, ambientLight.color.g, ambientLight.color.b);
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.intensity, ambientLight.intensity);
	}

	DirectionalLight* Scene::setDirectionalLight(glm::vec3 direction, float diffuseIntensity, glm::vec3 diffuseColor, glm::vec3 specularColor)
	{
		if (directionalLight != nullptr)
			delete directionalLight;
		directionalLight = new DirectionalLight(direction, diffuseIntensity, diffuseColor, specularColor);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.intensity, diffuseIntensity);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.direction, direction.x, direction.y, direction.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.diffuseColor, diffuseColor.r, diffuseColor.g, diffuseColor.b);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.specularColor, specularColor.r, specularColor.g, specularColor.b);
		return directionalLight;
	}
	PointLight* Scene::addPointLight(glm::vec3 position, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor)
	{
		PointLight *pointLight = new PointLight(position, power, quadraticFactor, linearFactor, constantFactor, diffuseColor, specularColor);
		pointLights.insert(pointLight);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.position, position.x, position.y, position.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.power, power);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.quadraticFactor, quadraticFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.linearFactor, linearFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.constantFactor, constantFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.diffuseColor, diffuseColor.x, diffuseColor.y, diffuseColor.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.specularColor, specularColor.x, specularColor.y, specularColor.z);
		return pointLight;
	}

	SpotLight* Scene::addSpotLight(glm::vec3 position, glm::vec3 direction, float innerAngleInDegrees, float outerAngleInDegrees, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor)
	{
		static const float PI = glm::pi<float>();

		float innerAngleCosine = std::cos(innerAngleInDegrees * PI / 180);
		float outerAngleCosine = std::cos(outerAngleInDegrees * PI / 180);
		SpotLight *spotLight = new SpotLight(position, direction, innerAngleInDegrees, outerAngleInDegrees, power, quadraticFactor, linearFactor, constantFactor, diffuseColor, specularColor);
		spotLights.insert(spotLight);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.position, position.x, position.y, position.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.direction, direction.x, direction.y, direction.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.innerAngleCosine, innerAngleCosine);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.outerAngleCosine, outerAngleCosine);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.power, power);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.quadraticFactor, quadraticFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.linearFactor, linearFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.constantFactor, constantFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.diffuseColor, diffuseColor.x, diffuseColor.y, diffuseColor.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.specularColor, specularColor.x, specularColor.y, specularColor.z);
		return spotLight;
	}

	glm::vec3 Scene::getAmbientLightColor()
	{
		return ambientLight.color;
	}
	float Scene::getAmbientLightIntensity()
	{
		return ambientLight.intensity;
	}
	void Scene::setAmbientLightColor(glm::vec3 color)
	{
		ambientLight.color = color;
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.color, ambientLight.color.r, ambientLight.color.g, ambientLight.color.b);
	}
	void Scene::setAmbientLightIntensity(float intensity)
	{
		if (intensity < 0)
			intensity = 0;
		ambientLight.intensity = intensity;
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.intensity, ambientLight.intensity);
	}

	void Scene::setPointLightPosition(glm::vec3 position)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.position, position.x, position.y, position.z);
	}

	void Scene::setSpotLightPosition(glm::vec3 position)
	{
		static SpotLight *light = *spotLights.begin();
		light->setPosition(position);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.position, position.x, position.y, position.z);
	}
}
