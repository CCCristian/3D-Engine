#include "Renderer.h"
#include "Options.h"

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
	void Renderer::draw(const Object& object)
	{
		const Model* model = object.getModel();
		for (int i = 0; i < model->getMeshObjectsCount(); i++)
		{
#ifndef USE_INSTANCED_RENDERING
			const Model::MeshObject& meshObj = *model->getMeshObjects()[i];
			glBindVertexArray(meshObj.mesh->vao);
			model->prepareShader(i);
			glm::mat4 transform = *object.getTransform() * meshObj.transform;
			glUniformMatrix4fv(SceneRenderingShader::uniformLocations.transform, 1, GL_FALSE, (const GLfloat *)&transform);
			glDrawElements(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
#else
			const Model::MeshObject& meshObj = *model->getMeshObjects()[i];
			model->prepareShader(i);
			glBindVertexArray(meshObj.mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, singleMatrixVBO);
			glm::mat4 transform = *object.getTransform() * meshObj.transform;
			glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(glm::mat4), &transform);
			glVertexAttribPointer(5, 4, GL_FLOAT, false, 0, 0);
			glVertexAttribPointer(6, 4, GL_FLOAT, false, 0, (const GLvoid *)sizeof(glm::vec4));
			glVertexAttribPointer(7, 4, GL_FLOAT, false, 0, (const GLvoid *)(2 * sizeof(glm::vec4)));
			glVertexAttribPointer(8, 4, GL_FLOAT, false, 0, (const GLvoid *)(3 * sizeof(glm::vec4)));
			glDrawElementsInstanced(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0, 1);
#endif
		}
	}
	void Renderer::draw(const ModelInstanceData& data, bool drawForShadowMap)
	{
#ifndef USE_INSTANCED_RENDERING
		const Model* model = data.getModel();
		for (int i = 0; i < model->getMeshObjectsCount(); i++)
		{
			const Model::MeshObject& meshObj = *model->getMeshObjects()[i];
			const std::vector<glm::mat4>& vec = data.getMeshInstancesVector()[i];
			if (!drawForShadowMap)
				model->prepareShader(i);
			glBindVertexArray(meshObj.mesh->vao);
			if (drawForShadowMap)
				for (int j = 0; j < data.getInstanceCount(); j++)
				{
					glUniformMatrix4fv(ShadowMapShader::uniformLocations.transform, 1, GL_FALSE, (const GLfloat *)&vec[j]);
					glDrawElements(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
				}
			else
			{
				for (int j = 0; j < data.getInstanceCount(); j++)
				{
					glUniformMatrix4fv(SceneRenderingShader::uniformLocations.transform, 1, GL_FALSE, (const GLfloat *)&vec[j]);
					glDrawElements(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0);
				}
			}
		}
#else
		const Model* model = data.getModel();
		data.updateVBOsToGPU();
		for (int i = 0; i < model->getMeshObjectsCount(); i++)
		{
			const Model::MeshObject& meshObj = *model->getMeshObjects()[i];
			if (!drawForShadowMap)
				model->prepareShader(i);
			glBindVertexArray(meshObj.mesh->vao);
			glBindBuffer(GL_ARRAY_BUFFER, data.getVBOHandles()[i]);
			glVertexAttribPointer(5, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), 0);
			glVertexAttribPointer(6, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (const GLvoid *)sizeof(glm::vec4));
			glVertexAttribPointer(7, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (const GLvoid *)(2 * sizeof(glm::vec4)));
			glVertexAttribPointer(8, 4, GL_FLOAT, false, 4 * sizeof(glm::vec4), (const GLvoid *)(3 * sizeof(glm::vec4)));
			glDrawElementsInstanced(GL_TRIANGLES, meshObj.mesh->getIndexCount(), GL_UNSIGNED_INT, 0, data.getMeshInstancesVector()[i].size());
		}
#endif
	}
	void Renderer::draw(Skybox* skybox, glm::mat4& world)
	{
		glBindVertexArray(skybox->vao);
		glActiveTexture(GL_TEXTURE0 + Shader::samplerValues.skyboxSampler);
		glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubemapHandle);
		glUniformMatrix4fv(SkyboxShader::uniformLocations.world, 1, GL_FALSE, (const GLfloat *)&world);
		glDrawElements(GL_TRIANGLES, skybox->indici.size(), GL_UNSIGNED_INT, 0);
	}
}