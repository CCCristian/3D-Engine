#include "Scene.h"
#include "Error.h"

namespace OpenGL
{
	std::set<Scene *> Scene::scenes;

	Scene::Scene(): sceneShader("main.vert", "main.frag"),
				shadowMapShader("shadowMap.vert", "shadowMap.frag"),
				skyboxShader("skybox.vert", "skybox.frag"),
				directShader("direct.vert", "direct.frag"),
				reflectionFramebuffer(Framebuffer::FramebufferType::Color),
				refractionFramebuffer(Framebuffer::FramebufferType::Color)
	{
		sceneShader.use();
		scenes.insert(this);

		camera = new Camera(16.f / 9.f, glm::pi<float>() / 3, 0.01f, 100.f);
		skybox = nullptr;

		// Init default ambient light
		glUniform3f(sceneShader.uniformLocations.ambientLight.color, 1, 1, 1);
		glUniform1f(sceneShader.uniformLocations.ambientLight.intensity, 0);

		// Init default directional light
		glUniform1f(sceneShader.uniformLocations.directionalLight.diffuseIntensity, 0);
		glUniform3f(sceneShader.uniformLocations.directionalLight.direction, 0, 0, -1);
		glUniform3f(sceneShader.uniformLocations.directionalLight.diffuseColor, 1, 1, 1);
		glUniform3f(sceneShader.uniformLocations.directionalLight.specularColor, 1, 1, 1);

		// Init default point light
		glUniform3f(sceneShader.uniformLocations.pointLight.position, 0, 0, 1);
		glUniform1f(sceneShader.uniformLocations.pointLight.power, 0);
		glUniform1f(sceneShader.uniformLocations.pointLight.quadraticFactor, 1);
		glUniform1f(sceneShader.uniformLocations.pointLight.linearFactor, 1);
		glUniform1f(sceneShader.uniformLocations.pointLight.constantFactor, 1);
		glUniform3f(sceneShader.uniformLocations.pointLight.diffuseColor, 1, 1, 1);
		glUniform3f(sceneShader.uniformLocations.pointLight.specularColor, 1, 1, 1);

		// Init default spot light
		glUniform3f(sceneShader.uniformLocations.spotLight.position, 0, 0, 1);
		glUniform3f(sceneShader.uniformLocations.spotLight.direction, 0, 0, -1);
		glUniform1f(sceneShader.uniformLocations.spotLight.innerAngleCosine, std::cos(15 * glm::pi<float>() / 180));
		glUniform1f(sceneShader.uniformLocations.spotLight.outerAngleCosine, std::cos(30 * glm::pi<float>() / 180));
		glUniform1f(sceneShader.uniformLocations.spotLight.power, 0);
		glUniform1f(sceneShader.uniformLocations.spotLight.quadraticFactor, 1);
		glUniform1f(sceneShader.uniformLocations.spotLight.linearFactor, 1);
		glUniform1f(sceneShader.uniformLocations.spotLight.constantFactor, 1);
		glUniform3f(sceneShader.uniformLocations.spotLight.diffuseColor, 1, 1, 1);
		glUniform3f(sceneShader.uniformLocations.spotLight.specularColor, 1, 1, 1);
	}
	Scene::~Scene()
	{
		delete camera;
		scenes.erase(this);
	}
	void Scene::drawSceneObjects(Renderer renderer, std::map<const Model*, ModelInstanceData*>& instances, bool drawForShadowMap)
	{
		for (auto it = instances.begin(), sf = instances.end(); it != sf; it++)
		{
			renderer.draw(*it->second, drawForShadowMap);
		}
	}
	void Scene::drawTextureOnScreen(Texture* texture, float x, float y, float width, float height)
	{
		glBindVertexArray(Model::getQuadVAO()->getHandle());
		glBindTexture(GL_TEXTURE_2D, texture->getHandle());
		glDepthMask(GL_FALSE);
		glDrawElements(GL_TRIANGLES, 6, GL_FLOAT, 0);
		glDepthMask(GL_TRUE);
	}
	void Scene::drawAll()
	{
		//static DirectionalLight *light = *directionalLights.begin();
		static SpotLight *light = *spotLights.begin();

		//static ShaderDirect* shad = []
		//{
		//	return new ShaderDirect();
		//}();
		//static Model *quad = [this]()
		//{
		//	Model *quad = Model::BaseModelGenerator::generateQuad("smap", light->getShadowFramebuffer()->getColorTexture());
		//	return quad;
		//}();
		//static Object* ob = new Object(quad);



		// ///////////////////////////////////////////////////////////////
		// ///////////////////////////////////////////////////////////////
		// ///////////////////////////////////////////////////////////////
		// ///////////////////////////////////////////////////////////////
		// ///////////////////////////////////////////////////////////////
		// ///////////////////////////////////////////////////////////////
		// ///////////////////////////////////////////////////////////////
		// ///////////////////////////////////////////////////////////////



		glBindFramebuffer(GL_FRAMEBUFFER, 0);
		glClearColor(1, 0, 0, 1);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		//glBindFramebuffer(GL_FRAMEBUFFER, light->getShadowFramebuffer()->getHandle());
		//glClearColor(0, 1, 0, 1);
		//glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		// Skybox rendering
		if (skybox != nullptr)
		{
			skyboxShader.use();
			glm::mat4 world = camera->getProjectionMatrix() * glm::mat4(glm::mat3(camera->getViewMatrix()));
			glDepthMask(GL_FALSE);
			renderer.draw(skybox, world);
			glDepthMask(GL_TRUE);
		}

		//// Shadow maps rendering pass
		shadowMapShader.use();
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, light->getShadowFramebuffer()->getHandle());
		glUniformMatrix4fv(ShadowMapShader::uniformLocations.lightMatrix, 1, GL_FALSE, (const GLfloat*)light->getShadowMapMatrix());
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		drawSceneObjects(renderer, instances, false);

		//static int done = 0;
		//if (done == 0)
		{
			//done = 1;
			float *buf = new float[1000 * 600];
			glBindTexture(GL_TEXTURE_2D, light->getShadowFramebuffer()->getDepthTexture()->getHandle());
			glGetTexImage(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, GL_FLOAT, buf);
			float first = buf[0];
			bool lafel = true;
			std::cout << buf[0] << '\n';
			for (int i = 0; i < 1000 * 600; i++)
			{
				if (buf[i] != first)
				{
					lafel = false;
					std::cout << buf[i] << '\n';
				}
			}
			delete[] buf;
			if (!lafel)
				std::cout << "Difera\n";
		}


		// Objects rendering pass
		sceneShader.use();
		glBindFramebuffer(GL_FRAMEBUFFER, light->getShadowFramebuffer()->getHandle());
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.shadowSampler);
		glBindTexture(GL_TEXTURE_2D, light->getShadowFramebuffer()->getDepthTexture()->getHandle());
		glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		glUniformMatrix4fv(sceneShader.uniformLocations.world, 1, GL_FALSE, (const GLfloat *)&camera->getWorldMatrix());
		glm::vec3 position = camera->getPosition();
		glUniform3f(sceneShader.uniformLocations.cameraPosition, position.x, position.y, position.z);
		glUniformMatrix4fv(sceneShader.uniformLocations.lightSpaceMatrix, 1, GL_FALSE, (const GLfloat *)light->getShadowMapMatrix());
		drawSceneObjects(renderer, instances, false);


		////glBindFramebuffer(GL_READ_FRAMEBUFFER, light->getShadowFramebuffer()->getHandle());
		////glBindFramebuffer(GL_DRAW_FRAMEBUFFER, 0);
		//glBindFramebuffer(GL_FRAMEBUFFER, 0);
		//shad->use();
		//glDepthMask(GL_FALSE);
		//renderer.draw(*ob);
		//glDepthMask(GL_TRUE);


		sceneShader.use();
#ifdef _DEBUG
		checkErrors();
#endif
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

	void Scene::setAmbientLight(glm::vec3 color, float intensity)
	{
		if (intensity < 0)
			intensity = 0;
		ambientLight.color = color;
		ambientLight.intensity = intensity;

		glUniform3f(sceneShader.uniformLocations.ambientLight.color, ambientLight.color.r, ambientLight.color.g, ambientLight.color.b);
		glUniform1f(sceneShader.uniformLocations.ambientLight.intensity, ambientLight.intensity);
	}

	void Scene::addDirectionalLight(glm::vec3 direction, float diffuseIntensity, glm::vec3 diffuseColor, glm::vec3 specularColor)
	{
		DirectionalLight *directionalLight = new DirectionalLight(direction, diffuseIntensity, diffuseColor, specularColor);
		directionalLights.insert(directionalLight);
		glUniform1f(sceneShader.uniformLocations.directionalLight.diffuseIntensity, diffuseIntensity);
		glUniform3f(sceneShader.uniformLocations.directionalLight.direction, direction.x, direction.y, direction.z);
		glUniform3f(sceneShader.uniformLocations.directionalLight.diffuseColor, diffuseColor.r, diffuseColor.g, diffuseColor.b);
		glUniform3f(sceneShader.uniformLocations.directionalLight.specularColor, specularColor.r, specularColor.g, specularColor.b);
	}
	void Scene::addPointLight(glm::vec3 position, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor)
	{
		PointLight *pointLight = new PointLight(position, power, quadraticFactor, linearFactor, constantFactor, diffuseColor, specularColor);
		pointLights.insert(pointLight);
		glUniform3f(sceneShader.uniformLocations.pointLight.position, position.x, position.y, position.z);
		glUniform1f(sceneShader.uniformLocations.pointLight.power, power);
		glUniform1f(sceneShader.uniformLocations.pointLight.quadraticFactor, quadraticFactor);
		glUniform1f(sceneShader.uniformLocations.pointLight.linearFactor, linearFactor);
		glUniform1f(sceneShader.uniformLocations.pointLight.constantFactor, constantFactor);
		glUniform3f(sceneShader.uniformLocations.pointLight.diffuseColor, diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(sceneShader.uniformLocations.pointLight.specularColor, specularColor.x, specularColor.y, specularColor.z);
	}

	void Scene::addSpotLight(glm::vec3 position, glm::vec3 direction, float innerAngleInDegrees, float outerAngleInDegrees, float power, float quadraticFactor, float linearFactor, float constantFactor, glm::vec3 diffuseColor, glm::vec3 specularColor)
	{
		static const float PI = glm::pi<float>();

		float innerAngleCosine = std::cos(innerAngleInDegrees * PI / 180);
		float outerAngleCosine = std::cos(outerAngleInDegrees * PI / 180);
		SpotLight *spotLight = new SpotLight(position, direction, innerAngleInDegrees, outerAngleInDegrees, power, quadraticFactor, linearFactor, constantFactor, diffuseColor, specularColor);
		spotLights.insert(spotLight);
		glUniform3f(sceneShader.uniformLocations.spotLight.position, position.x, position.y, position.z);
		glUniform3f(sceneShader.uniformLocations.spotLight.direction, direction.x, direction.y, direction.z);
		glUniform1f(sceneShader.uniformLocations.spotLight.innerAngleCosine, innerAngleCosine);
		glUniform1f(sceneShader.uniformLocations.spotLight.outerAngleCosine, outerAngleCosine);
		glUniform1f(sceneShader.uniformLocations.spotLight.power, power);
		glUniform1f(sceneShader.uniformLocations.spotLight.quadraticFactor, quadraticFactor);
		glUniform1f(sceneShader.uniformLocations.spotLight.linearFactor, linearFactor);
		glUniform1f(sceneShader.uniformLocations.spotLight.constantFactor, constantFactor);
		glUniform3f(sceneShader.uniformLocations.spotLight.diffuseColor, diffuseColor.x, diffuseColor.y, diffuseColor.z);
		glUniform3f(sceneShader.uniformLocations.spotLight.specularColor, specularColor.x, specularColor.y, specularColor.z);
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
		glUniform3f(sceneShader.uniformLocations.ambientLight.color, ambientLight.color.r, ambientLight.color.g, ambientLight.color.b);
	}
	void Scene::setAmbientLightIntensity(float intensity)
	{
		if (intensity < 0)
			intensity = 0;
		ambientLight.intensity = intensity;
		glUniform1f(sceneShader.uniformLocations.ambientLight.intensity, ambientLight.intensity);
	}

	void Scene::setPointLightPosition(glm::vec3 position)
	{
		glUniform3f(sceneShader.uniformLocations.pointLight.position, position.x, position.y, position.z);
	}

	void Scene::setSpotLightPosition(glm::vec3 position)
	{
		static SpotLight *light = *spotLights.begin();
		light->setPosition(position);
		glUniform3f(sceneShader.uniformLocations.spotLight.position, position.x, position.y, position.z);
	}
}
