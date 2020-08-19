#pragma once
#include "ICamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

class TPCamera : public ICamera
{
private:

	CameraObject _cameraObject;
	
	vec3 _rotation = glm::vec3();
	
	vec3 _position = glm::vec3();
	
	vec4 _viewPos = glm::vec4();
	
	float _fov;

	float _zNear;

	float _zFar;

	float _aspect;

public:

	TPCamera() = default;

	TPCamera(const TPCamera& other)
		: ICamera(other)
	{
	}

	TPCamera(TPCamera&& other) noexcept
		: ICamera(std::move(other))
	{
	}

	TPCamera& operator=(const TPCamera& other)
	{
		if (this == &other)
			return *this;
		ICamera::operator =(other);
		return *this;
	}

	TPCamera& operator=(TPCamera&& other) noexcept
	{
		if (this == &other)
			return *this;
		ICamera::operator =(std::move(other));
		return *this;
	}

	void SetPosition(vec3& position) override;
	
	void SetRotation(vec3& rotation) override;
	
	void Rotate(vec3& delta) override;
	
	void Move(vec3& delta) override;
	
	void SetPerspective(float fov, float aspect, float zNear, float zFar) override;
	void UpdateProjectionMatrix();

	void SetAspect(float aspect) override;
	
	float GetNear() override;
	
	float GetFar() override;

	CameraObject& GetCameraObject() override;
	
private:

	void UpdateViewMatrix();

};