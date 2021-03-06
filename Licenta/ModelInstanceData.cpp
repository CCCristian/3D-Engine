#include "pch.h"
#include "ModelInstanceData.h"

namespace OpenGL
{
	ModelInstanceData::ModelInstanceData(const Model *model): model(model), areVBOsUpdated(false)
	{
		int meshObjectsCount = model->getMeshObjectsCount();
		instanceVAOVectors.reserve(meshObjectsCount);
		instanceVBOHandles.reserve(meshObjectsCount);
		vbos.resize(meshObjectsCount);
		glGenBuffers(meshObjectsCount, &vbos[0]);
		for (int i = 0; i < meshObjectsCount; i++)
		{
			instanceVAOVectors.push_back(std::vector<glm::mat4>());
			instanceVBOHandles.push_back(vbos[i]);
#ifdef USE_INSTANCED_RENDERING
			glBindVertexArray(model->getMeshObjects()[i]->mesh->getHandle());
#endif
		}
	}
	ModelInstanceData::~ModelInstanceData()
	{
		glDeleteBuffers(model->getMeshObjectsCount(), &vbos[0]);
	}
	void ModelInstanceData::updateTransform(ModelInstanceData* modelInstanceData, Object *object)
	{
		// TODO: update vbos to GPU
		const std::vector<Model::MeshObject *>& meshObjects = object->getModel()->getMeshObjects();
		const int index = modelInstanceData->instanceIndices[object];
		for (int i = 0, end = object->getModel()->getMeshObjectsCount(); i < end; i++)
		{
			modelInstanceData->instanceVAOVectors[i][index] = *object->getTransform() * meshObjects[i]->transform;
		}
#ifdef USE_INSTANCED_RENDERING
		modelInstanceData->areVBOsUpdated = false;
#endif
	}
	void ModelInstanceData::addInstance(Object *object)
	{
		const std::vector<Model::MeshObject *>& meshObjects = model->getMeshObjects();
		for (int i = 0, end = model->getMeshObjectsCount(); i < end; i++)
		{
			instanceVAOVectors[i].push_back(*object->getTransform() * meshObjects[i]->transform);
		}
		instanceIndices.insert(std::make_pair(object, instanceVAOVectors[0].size() - 1));
		instancePointerVectors.push_back(object);
		object->setInstanceTransformUpdateData(this, updateTransform);
#ifdef USE_INSTANCED_RENDERING
		areVBOsUpdated = false;
#endif
	}
	void ModelInstanceData::removeInstance(Object *object)
	{
		const std::vector<Model::MeshObject *>& meshObjects = object->getModel()->getMeshObjects();
		auto it = instanceIndices.find(object);
		if (it == instanceIndices.end())
			return;
		const int index = it->second;
		const int lastIndex = instanceVAOVectors[0].size() - 1;
		const Object* lastObject = instancePointerVectors.back();
		for (int i = 0, end = object->getModel()->getMeshObjectsCount(); i < end; i++)
		{
			instanceVAOVectors[i][index] = instanceVAOVectors[i][lastIndex];
			instanceVAOVectors[i].pop_back();
		}
		instancePointerVectors[index] = lastObject;
		instancePointerVectors.pop_back();
		instanceIndices.erase(object);
		instanceIndices[lastObject] = index;
#ifdef USE_INSTANCED_RENDERING
		areVBOsUpdated = false;
#endif
	}
}
