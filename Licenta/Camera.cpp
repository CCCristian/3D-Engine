#include "Camera.h"

namespace OpenGL
{
	const glm::vec3 Camera::up(0.f, 0.f, 1.f);

	Camera::Camera()
	{
		position = glm::vec3(0, 0, 0);
		look = glm::vec3(0, 1, 0);
		vertAngle = horAngle = 0;
		calcAndSetVectors();
		updateView();
		setPerspective(glm::pi<float>() / 4, (float)16 / 4, (float)1, (float)10);
	}
	Camera::Camera(float aspectRatio, float verticalFOV, float nearZ, float drawDistance)
	{
		position = glm::vec3(0, 0, 0);
		look = glm::vec3(0, 1, 0);
		vertAngle = horAngle = 0;
		calcAndSetVectors();
		updateView();
		setPerspective(aspectRatio, verticalFOV, nearZ, nearZ + drawDistance);
	}
	glm::vec3 Camera::getPosition() const
	{
		return position;
	}
	Camera& Camera::setPosition(const glm::vec3 position)
	{
		Camera::position = position;
		updateView();
		return *this;
	}
	Camera& Camera::translateBy(const glm::vec3 translation)
	{
		Camera::position = Camera::position + translation;
		updateView();
		return *this;
	}
	Camera& Camera::moveForward(const float movement)
	{
		position += movement * look;
		updateView();
		return *this;
	}
	Camera& Camera::moveRight(const float movement)
	{
		position += movement * right;
		updateView();
		return *this;
	}
	Camera& Camera::moveUp(const float movement)
	{
		position += movement * up;
		updateView();
		return *this;
	}
	float Camera::getAspectRatio() const
	{
		return aspectRatio;
	}
	Camera& Camera::setAspectRatio(float ratio)
	{
		aspectRatio = ratio;
		setPerspective(aspectRatio, verticalFOV, nearZ, farZ);
		return *this;
	}
	float Camera::getFOV() const
	{
		return verticalFOV;
	}
	Camera& Camera::setFOV(float fov)
	{
		verticalFOV = fov;
		setPerspective(aspectRatio, verticalFOV, nearZ, farZ);
		return *this;
	}
	float Camera::getNearZ() const
	{
		return nearZ;
	}
	Camera& Camera::setNearZ(float z)
	{
		nearZ = z;
		setPerspective(aspectRatio, verticalFOV, nearZ, farZ);
		return *this;
	}
	float Camera::getFarZ() const
	{
		return farZ;
	}
	const glm::mat4& Camera::getWorldMatrix()
	{
		return world;
	}
	Camera& Camera::setFarZ(float z)
	{
		farZ = z;
		setPerspective(aspectRatio, verticalFOV, nearZ, farZ);
		return *this;
	}
	Camera& Camera::rotateVertical(const float angle)
	{
		vertAngle += sensitivity * angle;
		if (vertAngle > maxVertAngle)
			vertAngle = maxVertAngle;
		if (vertAngle < minVertAngle)
			vertAngle = minVertAngle;
		calcAndSetVectors();
		updateView();
		return *this;
	}
	Camera& Camera::rotateHorizontal(const float angle)
	{
		horAngle += sensitivity * angle;
		calcAndSetVectors();
		updateView();
		return *this;
	}
	Camera& Camera::calcAndSetVectors()
	{
		look = glm::vec3(-std::sin(horAngle), std::cos(horAngle), 0);
		look = glm::vec3(look.x*std::cos(vertAngle), look.y*std::cos(vertAngle), std::sin(vertAngle));
		right = glm::normalize(glm::cross(look, up));
		return *this;
	}
	void Camera::updateView()
	{
		view = glm::lookAt(position, look + position, up);
		world = perspective * view;
	}
	void Camera::setPerspective(float aspectRatio, float verticalFOV, float nearZ, float farZ)
	{
		Camera::aspectRatio = aspectRatio;
		Camera::verticalFOV = verticalFOV;
		Camera::nearZ = nearZ;
		Camera::farZ = farZ;
		perspective = glm::perspective(verticalFOV, aspectRatio, nearZ, farZ);
		world = perspective * view;
	}
	const glm::mat4& Camera::getViewMatrix()
	{
		return view;
	}
	const glm::mat4& Camera::getProjectionMatrix()
	{
		return perspective;
	}

	void Camera::mouseLookDelta(int x, int y)
	{
		rotateHorizontal((float)x);
		rotateVertical((float)y);
	}
	void Camera::invertPitch()
	{
		vertAngle = -vertAngle;
		calcAndSetVectors();
		updateView();
	}
}
