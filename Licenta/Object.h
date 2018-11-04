#pragma once

#include "GL.h"
#include "glm.hpp"
#include "glm\gtc\matrix_transform.hpp"
#include "glm\gtc\constants.hpp"
#include "glm\gtc\quaternion.hpp"
#include "glm\gtx\quaternion.hpp"
#include "Shader.h"
#include "Model.h"

namespace OpenGL
{
	class Model;
	class ModelInstanceData;
	class Object
	{
		//friend class Model;
		//friend class Scene;
		const Model *model;
		void updateTransform();
	protected:
		glm::vec3 position;
		glm::quat rotation;
		glm::vec3 scale;
		glm::mat4 transform;
		ModelInstanceData *modelInstanceData;
		void (*instanceTransformUpdateCallback)(ModelInstanceData *, Object *);
	public:
		Object(const Model *model);

		const Model* getModel() const			{ return model; }
		const glm::vec3 getPosition() const		{ return position; }
		const glm::quat getRotation() const		{ return rotation; }
		const glm::vec3 getScale() const		{ return scale; }
		const glm::mat4* getTransform() const	{ return &transform; }

		Object& setPosition(float x, float y, float z);
		Object& setPosition(const glm::vec3 position);
		Object& setRotation(const glm::vec3 eulerAngles);
		Object& setTransform(const glm::mat4 transform);
		Object& setScale(const float scale);
		Object& setScale(float x, float y, float z);
		Object& setScale(const glm::vec3 scale);
		Object& setInstanceTransformUpdateData(ModelInstanceData *, void (*instanceTransformUpdateCallback)(ModelInstanceData *, Object *));

		Object& translateBy(const glm::vec3 translation);
		Object& rotateX(const float angle);
		Object& rotateY(const float angle);
		Object& rotateZ(const float angle);
		Object& scaleBy(const glm::vec3 scale);
	};
}
