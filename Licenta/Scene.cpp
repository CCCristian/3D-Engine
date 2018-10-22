#include "Scene.h"
#include "Error.h"
#include "glm/gtc/matrix_transform.hpp"
#include <cmath>

namespace OpenGL
{
	std::set<Scene *> Scene::scenes;

	Scene::Scene(): camera(new Camera(16.f / 9.f, glm::pi<float>() / 3, 0.1f, 100.f)), drawer(*camera, lights), skybox(nullptr)
	{
		scenes.insert(this);
	}

	Scene::~Scene()
	{
		delete camera;
		scenes.erase(this);
	}

	void Scene::drawAll()
	{
		// Clear all framebuffers
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		glBindFramebuffer(GL_FRAMEBUFFER, lights.directionalLight->getShadowFramebuffer()->getHandle());
		glClear(GL_DEPTH_BUFFER_BIT);
		for (Water* water : waterBodies)
		{
			water->getReflectionFramebuffer().useAndClear();
			water->getRefractionFramebuffer().useAndClear();
		}

		// Skybox rendering
		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		drawer.drawSkybox(*skybox);
		camera->invertPitch();
		for (Water* water : waterBodies)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, water->getReflectionFramebuffer().getHandle());
			drawer.drawSkybox(*skybox);
		}
		camera->invertPitch();
		for (Water* water : waterBodies)
		{
			glBindFramebuffer(GL_FRAMEBUFFER, water->getRefractionFramebuffer().getHandle());
			drawer.drawSkybox(*skybox);
		}

		// Objects rendering
		drawer.drawSceneObjects(instances);

		// Water rendering
		for (Water* water : waterBodies)
		{
			drawer.drawWater(instances, *water);
		}

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

	void Scene::setAmbientLight(const glm::vec3& color, float intensity)
	{
		if (intensity < 0)
			intensity = 0;
		lights.ambientLight.color = color;
		lights.ambientLight.intensity = intensity;
		drawer.setAmbientLight(lights.ambientLight);
	}

	DirectionalLight* Scene::setDirectionalLight(const glm::vec3& direction, float diffuseIntensity, const glm::vec3& diffuseColor, const glm::vec3& specularColor)
	{
		if (lights.directionalLight != nullptr)
			delete lights.directionalLight;
		lights.directionalLight = new DirectionalLight(direction, diffuseIntensity, diffuseColor, specularColor);
		drawer.setDirectionalLight(*lights.directionalLight);
		return lights.directionalLight;
	}

	PointLight* Scene::addPointLight(const glm::vec3& position, float power, float quadraticFactor, float linearFactor, float constantFactor, const glm::vec3& diffuseColor, const glm::vec3& specularColor)
	{
		PointLight *pointLight = new PointLight(position, power, quadraticFactor, linearFactor, constantFactor, diffuseColor, specularColor);
		lights.pointLights.insert(pointLight);
		drawer.addPointLight(*pointLight);
		return pointLight;
	}

	SpotLight* Scene::addSpotLight(const glm::vec3& position, const glm::vec3& direction, float innerAngleInDegrees, float outerAngleInDegrees, float power, float quadraticFactor, float linearFactor, float constantFactor, const glm::vec3& diffuseColor, const glm::vec3& specularColor)
	{
		static const float PI = glm::pi<float>();

		float innerAngleCosine = std::cos(innerAngleInDegrees * PI / 180);
		float outerAngleCosine = std::cos(outerAngleInDegrees * PI / 180);
		SpotLight *spotLight = new SpotLight(position, direction, innerAngleInDegrees, outerAngleInDegrees, power, quadraticFactor, linearFactor, constantFactor, diffuseColor, specularColor);
		lights.spotLights.insert(spotLight);
		drawer.addSpotLight(*spotLight);
		return spotLight;
	}

	glm::vec3 Scene::getAmbientLightColor()
	{
		return lights.ambientLight.color;
	}
	float Scene::getAmbientLightIntensity()
	{
		return lights.ambientLight.intensity;
	}
	void Scene::setAmbientLightColor(const glm::vec3& color)
	{
		lights.ambientLight.color = color;
		drawer.setAmbientLightColor(color);
	}
	void Scene::setAmbientLightIntensity(float intensity)
	{
		if (intensity < 0)
			intensity = 0;
		lights.ambientLight.intensity = intensity;
		drawer.setAmbientLightIntensity(intensity);
	}

	void Scene::setPointLightPosition(const glm::vec3& position)
	{
		drawer.setPointLightPosition(position);
	}

	void Scene::setSpotLightPosition(const glm::vec3& position)
	{
		static SpotLight *light = *lights.spotLights.begin();
		light->setPosition(position);
		drawer.setSpotLightPosition(position);
	}

	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////////

	Scene::Drawer::Drawer(Camera& camera, const Lights& lights): camera(camera), lights(lights), activeShader(nullptr),
		sceneShader("main.vert", "main.frag"),
		shadowMapShader("shadowMap.vert", "shadowMap.frag"),
		skyboxShader("skybox.vert", "skybox.frag"),
		waterShader("water.vert", "water.frag"),
		directShader("direct.vert", "direct.frag")
	{
		setActiveShader(sceneShader);
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

	void Scene::Drawer::drawSceneObjects(const decltype(instances) instances, const Framebuffer* framebuffer)
	{
		if (activeShader != &shadowMapShader)
			setActiveShader(shadowMapShader);
		// Shadow maps rendering pass
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, lights.directionalLight->getShadowFramebuffer()->getHandle());
		glUniformMatrix4fv(ShadowMapShader::uniformLocations.lightMatrix, 1, GL_FALSE, (const GLfloat*)lights.directionalLight->getShadowMapMatrix());
		glClear(GL_DEPTH_BUFFER_BIT);
		for (auto it = instances.begin(), sf = instances.end(); it != sf; it++)
		{
			renderer.draw(shadowMapShader, *it->second);
		}
		checkErrors();


		// Objects rendering pass
		setActiveShader(sceneShader);
		glBindFramebuffer(GL_READ_FRAMEBUFFER, lights.directionalLight->getShadowFramebuffer()->getHandle());
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.shadowSampler);
		glBindTexture(GL_TEXTURE_2D, lights.directionalLight->getShadowFramebuffer()->getDepthTexture()->getHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, framebuffer == nullptr ? 0 : framebuffer->getHandle());
		sceneShader.loadUniform(sceneShader.uniformLocations.world, camera.getWorldMatrix());
		glm::vec3 position = camera.getPosition();
		sceneShader.loadUniform(sceneShader.uniformLocations.cameraPosition, position.x, position.y, position.z);
		glUniformMatrix4fv(sceneShader.uniformLocations.lightSpaceMatrix, 1, GL_FALSE, (const GLfloat *)lights.directionalLight->getShadowMapMatrix());
		for (auto it = instances.begin(), sf = instances.end(); it != sf; it++)
		{
			renderer.draw(sceneShader, *it->second);
		}
		checkErrors();
	}

	void Scene::Drawer::drawTextureOnScreen(const Texture* texture, float x, float y, float width, float height)
	{
		static GLuint vao = Model::getQuadModel()->getMeshes()[0]->getHandle();

		glBindVertexArray(vao);
		glActiveTexture(GL_TEXTURE0 + DirectShader::uniformLocations.colorSampler);
		if (activeShader != &directShader)
			setActiveShader(directShader);
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

	void Scene::Drawer::drawSkybox(const Skybox& skybox)
	{
		if (activeShader != &skyboxShader)
			setActiveShader(skyboxShader);
		glm::mat4 world = camera.getProjectionMatrix() * glm::mat4(glm::mat3(camera.getViewMatrix()));
		glDepthMask(GL_FALSE);
		renderer.draw(skyboxShader, &skybox, world);
		glDepthMask(GL_TRUE);
	}

	void Scene::Drawer::drawSkybox(const Skybox& skybox, const glm::mat4& projectionMatrix, const glm::mat4& viewMatrix)
	{
		if (activeShader != &skyboxShader)
			setActiveShader(skyboxShader);
		glm::mat4 world = projectionMatrix * glm::mat4(glm::mat3(viewMatrix));
		glDepthMask(GL_FALSE);
		renderer.draw(skyboxShader, &skybox, world);
		glDepthMask(GL_TRUE);
	}

	void Scene::Drawer::drawWater(const decltype(instances) reflectedObjects, const Water& water)
	{
		if (activeShader != &sceneShader)
			setActiveShader(sceneShader);
		static float movement = 0.0f;
		movement = std::fmod((float)TimeManager::elapsedMilliseconds() / 35000, 1.f);
		glm::vec3 initialCameraPosition = camera.getPosition();

		// Render reflection
		camera.setPosition(glm::vec3(initialCameraPosition.x, initialCameraPosition.y, 2 * water.getHeight() - initialCameraPosition.z));
		camera.invertPitch();
		glm::vec3 position = camera.getPosition();
		sceneShader.loadUniform(sceneShader.uniformLocations.clipPlane, 0, 0, 1, -(water.getHeight()));
		sceneShader.loadUniform(sceneShader.uniformLocations.cameraPosition, position.x, position.y, position.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.world, camera.getWorldMatrix());
		water.getReflectionFramebuffer().bind();
		glEnable(GL_CLIP_DISTANCE0);
		drawSceneObjects(reflectedObjects, &water.getReflectionFramebuffer());
		glDisable(GL_CLIP_DISTANCE0);

		// Render refraction
		camera.setPosition(initialCameraPosition);
		camera.invertPitch();
		sceneShader.loadUniform(sceneShader.uniformLocations.clipPlane, 0, 0, -1, water.getHeight() + 0.1f);
		position = camera.getPosition();
		sceneShader.loadUniform(sceneShader.uniformLocations.cameraPosition, position.x, position.y, position.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.world, camera.getWorldMatrix());
		water.getRefractionFramebuffer().bind();
		glEnable(GL_CLIP_DISTANCE0);
		drawSceneObjects(reflectedObjects, &water.getRefractionFramebuffer());
		glDisable(GL_CLIP_DISTANCE0);

		// Combining reflection with refraction
		glBindFramebuffer(GL_FRAMEBUFFER, 0);


		setActiveShader(waterShader);
		sceneShader.loadUniform(waterShader.uniformLocations.world, camera.getWorldMatrix());
		waterShader.loadUniform(waterShader.uniformLocations.cameraPosition, camera.getPosition());
		waterShader.loadUniform(waterShader.uniformLocations.directionalLight, lights.directionalLight->getDirection());
		waterShader.loadUniform(waterShader.uniformLocations.directionalColor, lights.directionalLight->getSpecularColor());
		waterShader.loadUniform(waterShader.uniformLocations.movement, movement);
		renderer.draw(waterShader, water);
		checkErrors();
	}

	void Scene::Drawer::setAmbientLight(const AmbientLight& ambientLight)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.color, ambientLight.color.r, ambientLight.color.g, ambientLight.color.b);
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.intensity, ambientLight.intensity);
	}

	void Scene::Drawer::setDirectionalLight(const DirectionalLight& directionalLight)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.intensity, directionalLight.diffuseIntensity);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.direction, directionalLight.direction.x, directionalLight.direction.y, directionalLight.direction.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.diffuseColor, directionalLight.diffuseColor.r, directionalLight.diffuseColor.g, directionalLight.diffuseColor.b);
		sceneShader.loadUniform(sceneShader.uniformLocations.directionalLight.specularColor, directionalLight.specularColor.r, directionalLight.specularColor.g, directionalLight.specularColor.b);
	}

	void Scene::Drawer::addPointLight(const PointLight& pointLight)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.position, pointLight.position.x, pointLight.position.y, pointLight.position.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.power, pointLight.power);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.quadraticFactor, pointLight.quadraticFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.linearFactor, pointLight.linearFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.constantFactor, pointLight.constantFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.diffuseColor, pointLight.diffuseColor.x, pointLight.diffuseColor.y, pointLight.diffuseColor.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.specularColor, pointLight.specularColor.x, pointLight.specularColor.y, pointLight.specularColor.z);
	}

	void Scene::Drawer::addSpotLight(const SpotLight& spotLight)
	{
		static constexpr float PI = glm::pi<float>();

		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.position, spotLight.position.x, spotLight.position.y, spotLight.position.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.direction, spotLight.direction.x, spotLight.direction.y, spotLight.direction.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.innerAngleCosine, spotLight.innerAngleCosine);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.outerAngleCosine, spotLight.outerAngleCosine);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.power, spotLight.power);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.quadraticFactor, spotLight.quadraticFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.linearFactor, spotLight.linearFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.constantFactor, spotLight.constantFactor);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.diffuseColor, spotLight.diffuseColor.x, spotLight.diffuseColor.y, spotLight.diffuseColor.z);
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.specularColor, spotLight.specularColor.x, spotLight.specularColor.y, spotLight.specularColor.z);
	}

	void Scene::Drawer::setAmbientLightColor(const glm::vec3& color)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.color, lights.ambientLight.color.r, lights.ambientLight.color.g, lights.ambientLight.color.b);
	}

	void Scene::Drawer::setAmbientLightIntensity(float intensity)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.ambientLight.intensity, lights.ambientLight.intensity);
	}

	void Scene::Drawer::setPointLightPosition(const glm::vec3& position)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.pointLight.position, position.x, position.y, position.z);
	}

	void Scene::Drawer::setSpotLightPosition(const glm::vec3& position)
	{
		sceneShader.loadUniform(sceneShader.uniformLocations.spotLight.position, position.x, position.y, position.z);
	}
}
