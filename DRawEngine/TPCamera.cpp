#include "TPCamera.h"



void TPCamera::SetPosition(vec3& position)
{
	_position = position;
	UpdateViewMatrix();
}

void TPCamera::SetRotation(vec3& rotation)
{
	_rotation = rotation;
	UpdateViewMatrix();
}

void TPCamera::Rotate(vec3& delta)
{
	_rotation += delta;
	UpdateViewMatrix();
}

void TPCamera::Move(vec3& delta)
{
	_position += delta;
	UpdateViewMatrix();
}

void TPCamera::SetPerspective(float fov, float aspect, float zNear, float zFar)
{
	_fov = fov;
	_aspect = aspect;
	_zNear = zNear;
	_zFar = zFar;
	UpdateProjectionMatrix();
}

void TPCamera::UpdateProjectionMatrix()
{
	_cameraObject._proj = perspective(glm::radians(_fov), _aspect, _zNear, _zFar);
}

void TPCamera::SetAspect(float aspect)
{
	_aspect = aspect;
	UpdateProjectionMatrix();
}

float TPCamera::GetNear()
{
	return _zNear;
}

float TPCamera::GetFar()
{
	return _zFar;
}

CameraObject& TPCamera::GetCameraObject()
{
	return _cameraObject;
}

void TPCamera::UpdateViewMatrix()
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
