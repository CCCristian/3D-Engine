#include "Object.h"


namespace OpenGL
{
	Object::Object(const Model *model)
	{
		Object::model = model;
		position = glm::vec3(0, 0, 0);
		scale = glm::vec3(1, 1, 1);
		instanceTransformUpdateCallback = nullptr;
		updateTransform();
	}
	void Object::updateTransform()
	{
		transform = glm::translate(glm::mat4(1), position) * glm::toMat4(rotation) * glm::scale(glm::mat4(1), scale);
		if (instanceTransformUpdateCallback != nullptr)
			instanceTransformUpdateCallback(modelInstanceData, this);
	}
	Object& Object::setPosition(glm::vec3 position)
	{
		Object::position = position;
		updateTransform();
		return *this;
	}
	Object& Object::setPosition(float x, float y, float z)
	{
		Object::position = glm::vec3(x, y, z);
		updateTransform();
		return *this;
	}
	Object& Object::setRotation(const glm::vec3 eulerAngles)
	{
		rotation = glm::quat(eulerAngles);
		updateTransform();
		return *this;
	}
	Object& Object::setTransform(const glm::mat4 transform)
	{
		Object::transform = transform;
		if (instanceTransformUpdateCallback != nullptr)
			instanceTransformUpdateCallback(modelInstanceData, this);
		return *this;
	}
	Object& Object::setScale(const float scale)
	{
		Object::scale = glm::vec3(scale);
		updateTransform();
		return *this;
	}
	Object& Object::setScale(float x, float y, float z)
	{
		Object::scale = glm::vec3(x, y, z);
		updateTransform();
		return *this;
	}
	Object& Object::setScale(const glm::vec3 scale)
	{
		Object::scale = scale;
		updateTransform();
		return *this;
	}
	Object& Object::setInstanceTransformUpdateData(ModelInstanceData *modelInstanceData, void (*instanceTransformUpdateCallback)(ModelInstanceData *, Object *))
	{
		Object::modelInstanceData = modelInstanceData;
		Object::instanceTransformUpdateCallback = instanceTransformUpdateCallback;
		return *this;
	}
	Object& Object::translateBy(const glm::vec3 translation)
	{
		Object::position = Object::position + translation;
		updateTransform();
		return *this;
	}
	Object& Object::rotateX(const float angle)
	{
		rotation = glm::quat(glm::vec3(angle, 0, 0)) * rotation;
		updateTransform();
		return *this;
	}
	Object& Object::rotateY(const float angle)
	{
		rotation = glm::quat(glm::vec3(0, angle, 0)) * rotation;
		updateTransform();
		return *this;
	}
	Object& Object::rotateZ(const float angle)
	{
		rotation = glm::quat(glm::vec3(0, 0, angle)) * rotation;
		updateTransform();
		return *this;
	}
	Object& Object::scaleBy(const glm::vec3 scale)
	{
		Object::scale = Object::scale * scale;
		updateTransform();
		return *this;
	}
}
