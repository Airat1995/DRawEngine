#pragma once
#include "ICamera.h"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

template<typename T>
class TPCamera : public ICamera<CameraObject>
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

	float _speed;

public:

	TPCamera(float cameraSpeed)
	{
		_speed = cameraSpeed;
	}

	~TPCamera()
	{
	}

	

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

	void SetPosition(vec3& position) override
	{
		_position = position;
		UpdateViewMatrix();
	}

	void SetRotation(vec3& rotation) override
	{
		_rotation = rotation;
		UpdateViewMatrix();
	}

	void Rotate(vec3& delta) override
	{
		_rotation += delta;
		UpdateViewMatrix();
	}

	void Move(vec3& delta) override
	{
		_position += delta * _speed;
		UpdateViewMatrix();
	}

	void SetPerspective(float fov, float aspect, float zNear, float zFar) override
	{
		_fov = fov;
		_aspect = aspect;
		_zNear = zNear;
		_zFar = zFar;
		UpdateProjectionMatrix();
	}

	void UpdateProjectionMatrix()
	{
		_cameraObject._proj = perspective(glm::radians(_fov), _aspect, _zNear, _zFar);
	}

	void SetAspect(float aspect) override
	{
		_aspect = aspect;
		UpdateProjectionMatrix();
	}

	mat4 GetViewModel()
	{
		mat4 viewModel = _cameraObject._view * _cameraObject._model;
		return viewModel;
	}

	float GetNear() override
	{
		return _zNear;
	}

	float GetFar() override
	{
		return _zFar;
	}

	vec3 GetPosition() override
	{
		return _position;
	}

	CameraObject& GetCameraObject() override
	{
		return _cameraObject;
	}

private:

	void UpdateViewMatrix()
	{
		mat4 rotM = mat4(1.0f);

		rotM = rotate(rotM, radians(_rotation.x * (1.0f)), glm::vec3(1.0f, 0.0f, 0.0f));
		rotM = rotate(rotM, radians(_rotation.y), glm::vec3(0.0f, 1.0f, 0.0f));
		rotM = rotate(rotM, radians(_rotation.z), glm::vec3(0.0f, 0.0f, 1.0f));

		const vec3 translation = _position;
		mat4 transM = glm::translate(mat4(1.0f), translation);

		_cameraObject._view = transM * rotM;

		_viewPos = glm::vec4(_position, 0.0f) * glm::vec4(-1.0f, 1.0f, -1.0f, 1.0f);
	}
};