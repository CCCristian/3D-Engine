#pragma once

#include "GL.h"

namespace OpenGL
{
	class Camera
	{
		static const glm::vec3 up;
		const float sensitivity = 0.001f;
		const float maxVertAngle = 85 * glm::pi<float>() / 180;
		const float minVertAngle = -85 * glm::pi<float>() / 180;
		glm::vec3 position;
		glm::vec3 look;
		glm::vec3 right;
		glm::mat4 view;
		glm::mat4 perspective;
		glm::mat4 world;
		float aspectRatio;
		float verticalFOV;
		float nearZ;
		float farZ;
		float vertAngle;
		float horAngle;
		void updateView();
		Camera& calcAndSetVectors();

	public:
		Camera();
		Camera(float aspectRatio, float verticalFOV, float nearZ, float drawDistance);
		glm::vec3 getPosition() const;
		Camera& setPosition(const glm::vec3 position);
		Camera& rotateVertical(const float angle);
		Camera& rotateHorizontal(const float angle);
		Camera& translateBy(const glm::vec3 translation);
		Camera& moveForward(const float movement);
		Camera& moveRight(const float movement);
		Camera& moveUp(const float movement);
		float getAspectRatio() const;
		float getFOV() const;
		float getNearZ() const;
		float getFarZ() const;
		glm::vec3 getLookVector()	{ return look; }
		const glm::mat4& getWorldMatrix();
		Camera& setAspectRatio(float);
		Camera& setFOV(float);
		Camera& setNearZ(float);
		Camera& setFarZ(float);
		void setPerspective(float aspectRatio, float verticalFOV, float nearZ, float farZ);
		const glm::mat4& getViewMatrix();
		const glm::mat4& getProjectionMatrix();
		void mouseLookDelta(int x, int y);
		void invertPitch();
	};
}
