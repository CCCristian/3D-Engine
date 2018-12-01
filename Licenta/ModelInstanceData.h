#pragma once

#include <vector>
#include "glm.hpp"
#include "Object.h"
#include "Options.h"

namespace OpenGL
{
	class ModelInstanceData
	{
	//public:
	//	typedef std::vector<glm::mat4> MeshInstancesVector;

	private:
		/** The model whose instances are being kept. */
		const Model* model;
		/** The instance vbos handles. */
		std::vector<GLuint> vbos;
		/** Indexed by mesh index, contains the matrix for the current indexed mesh. */
		std::vector<std::vector<glm::mat4>> instanceVAOVectors;
		/** Contains the object pointer for the given index. */
		std::vector<GLuint> instanceVBOHandles;
		/** Contains the object pointer for the given index. */
		std::vector<const Object *> instancePointerVectors;
		/** The index of the given object in the instance vectors. */
		std::map<const Object *, int> instanceIndices;

		static void updateTransform(ModelInstanceData* modelInstanceData, Object *object);
	public:
		ModelInstanceData(const Model *model);
		~ModelInstanceData();
		void addInstance(Object *obj);
		void removeInstance(Object *obj);
		const Model* getModel() const					{ return model; }
		const std::vector<std::vector<glm::mat4>>& getMeshInstancesVector() const		{ return instanceVAOVectors; }
		int getInstanceCount() const					{ return instancePointerVectors.size(); }
		const std::vector<GLuint> getVBOHandles() const	{ return vbos; }
#ifdef USE_INSTANCED_RENDERING
		/** This is to orphan VBOs only when needed, on render, instead of on each instances vector change. */
		mutable bool areVBOsUpdated;
#endif
	};
}
