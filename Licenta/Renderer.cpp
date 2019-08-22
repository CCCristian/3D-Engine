#include "pch.h"
#include "Renderer.h"
#include "Options.h"
#include "Debug.h"

namespace OpenGL
{
	Renderer::Renderer()
	{
		glGenBuffers(1, &singleMatrixVBO);
		glBindBuffer(GL_ARRAY_BUFFER, singleMatrixVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::mat4), 0, GL_DYNAMIC_DRAW);
	}

	Renderer::~Renderer()
	{
		glDeleteBuffers(1, &singleMatrixVBO);
	}

	void Renderer::draw(const Shader& shader, const Object& object)
	{
		const Model* model = object.getModel();
		for (int i = 0; i < model->getMeshObjectsCount(); i++)
		{
			const Model::MeshObject& meshObj = *model->getMeshObjects()[i];
			glBindVertexArray(meshObj.mesh->getHandle());
			model->prepareShader(i, shader);
			glm::mat4 transform = *object.getTransform() * meshObj.transform;

#ifndef USE_INSTANCED_RENDERING
			GLuint location = SceneRenderingShader::uniformLocations.transform;
			if (shader.shaderType == Shader::ShaderType::WaterShader)
				location = WaterShader::uniformLocations.transform;
			glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat *)&transform);
			glDrawElements(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
#else
			glBindBuffer(GL_ARRAY_BUFFER, singleMatrixVBO);
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &transform);
			glVertexAttribPointer(5, 4, GL_FLOAT, false, 0, 0);
			glVertexAttribPointer(6, 4, GL_FLOAT, false, 0, (const GLvoid *)sizeof(glm::vec4));
			glVertexAttribPointer(7, 4, GL_FLOAT, false, 0, (const GLvoid *)(2 * sizeof(glm::vec4)));
			glVertexAttribPointer(8, 4, GL_FLOAT, false, 0, (const GLvoid *)(3 * sizeof(glm::vec4)));
			glDrawElementsInstanced(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0, 1);
#endif

			checkErrors();
		}
	}

	void Renderer::draw(const Shader& shader, const Water& water)
	{
		static GLuint quadVAO = Model::getQuadModel()->getMeshes()[0]->getHandle();

		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.reflectionSampler);
		glBindTexture(GL_TEXTURE_2D, water.getReflectionFramebuffer().getColorTexture()->getHandle());
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.refractionSampler);
		glBindTexture(GL_TEXTURE_2D, water.getRefractionFramebuffer().getColorTexture()->getHandle());
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.dudvSampler);
		glBindTexture(GL_TEXTURE_2D, Water::getDUDVMap()->getHandle());
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.normalSampler);
		glBindTexture(GL_TEXTURE_2D, Water::getNormalMap()->getHandle());
		glUniform2f(WaterShader::uniformLocations.tiles, water.getTiles().x, water.getTiles().y);


#ifndef USE_INSTANCED_RENDERING
		glBindVertexArray(quadVAO);
		glUniformMatrix4fv(WaterShader::uniformLocations.transform, 1, GL_FALSE, (const GLfloat *)water.getTransform());
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
		checkErrors();
#else
		glBindVertexArray(quadVAO);
		glBindBuffer(GL_ARRAY_BUFFER, singleMatrixVBO);
		glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), water.getTransform());
		glVertexAttribPointer(5, 4, GL_FLOAT, false, 0, 0);
		glVertexAttribPointer(6, 4, GL_FLOAT, false, 0, (const GLvoid *)sizeof(glm::vec4));
		glVertexAttribPointer(7, 4, GL_FLOAT, false, 0, (const GLvoid *)(2 * sizeof(glm::vec4)));
		glVertexAttribPointer(8, 4, GL_FLOAT, false, 0, (const GLvoid *)(3 * sizeof(glm::vec4)));
		glDrawElementsInstanced(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0, 1);
		checkErrors();
#endif
	}

	void Renderer::draw(const Shader& shader, const ModelInstanceData& data)
	{
		const Model* model = data.getModel();
#ifdef USE_INSTANCED_RENDERING
		updateVBOsToGPU(data);
#endif
		for (int i = 0; i < model->getMeshObjectsCount(); i++)
		{
			const Model::MeshObject& meshObj = *model->getMeshObjects()[i];
			model->prepareShader(i, shader);
			glBindVertexArray(meshObj.mesh->getHandle());
			const std::vector<glm::mat4>& vec = data.getMeshInstancesVector()[i];

#ifndef USE_INSTANCED_RENDERING
			GLuint location = SceneRenderingShader::uniformLocations.transform;
			if (shader.shaderType == Shader::ShaderType::WaterShader)
				location = WaterShader::uniformLocations.transform;
			if (shader.shaderType == Shader::ShaderType::ShadowMapShader)
				location = ShadowMapShader::uniformLocations.transform;
			for (int j = 0; j < data.getInstanceCount(); j++)
			{
				glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat *)&vec[j]);
				glDrawElements(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
			}
			for (int j = 0; j < data.getInstanceCount(); j++)
			{
				glUniformMatrix4fv(location, 1, GL_FALSE, (const GLfloat *)&vec[j]);
				glDrawElements(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
			}
#else
			glBindBuffer(GL_ARRAY_BUFFER, data.getVBOHandles()[i]);
			glVertexAttribPointer(5, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), 0);
			glVertexAttribPointer(6, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (const GLvoid *)sizeof(glm::vec4));
			glVertexAttribPointer(7, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (const GLvoid *)(2 * sizeof(glm::vec4)));
			glVertexAttribPointer(8, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (const GLvoid *)(3 * sizeof(glm::vec4)));
			glDrawElementsInstanced(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0, data.getMeshInstancesVector()[i].size());
#endif
		}
	}

	void Renderer::draw(const Shader& shader, const Skybox* skybox, const glm::mat4& world)
	{
		glBindVertexArray(skybox->vao);
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.skyboxSampler);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemapHandle);
		glUniformMatrix4fv(SkyboxShader::uniformLocations.world, 1, GL_FALSE, reinterpret_cast<const GLfloat *>(&world));
		glDrawElements(GL_TRIANGLES, skybox->indici.size(), GL_UNSIGNED_INT, 0);
		glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	}

#ifdef USE_INSTANCED_RENDERING
	void Renderer::updateVBOsToGPU(const ModelInstanceData& data) const
	{
		if (data.areVBOsUpdated)
			return;
		data.areVBOsUpdated = true;

		const std::vector<Model::MeshObject *>& meshObjects = data.getModel()->getMeshObjects();
		for (int i = 0, end = data.getModel()->getMeshObjectsCount(); i < end; i++)
		{
			glBindBuffer(GL_ARRAY_BUFFER, data.getVBOHandles()[i]);
			glBufferData(GL_ARRAY_BUFFER, data.getMeshInstancesVector()[i].size() * sizeof(glm::mat4), 0, GL_DYNAMIC_DRAW);
			glBufferSubData(GL_ARRAY_BUFFER, 0, data.getMeshInstancesVector()[i].size() * sizeof(glm::mat4), &data.getMeshInstancesVector()[i][0]);
		}
	}
#endif
}
