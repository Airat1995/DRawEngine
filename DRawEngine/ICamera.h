#pragma once
#include "CameraObject.h"

using namespace glm;

class ICamera
{
public:
	virtual void SetPosition(vec3& position) = 0;

	virtual void SetRotation(vec3& rotation) = 0;

	virtual void Rotate(vec3& delta) = 0;

	virtual void Move(vec3& delta) = 0;

	virtual void SetPerspective(float fov, float aspect, float zNear, float zFar) = 0;

	virtual void SetAspect(float aspect) = 0;

	virtual float GetNear() = 0;

	virtual float GetFar() = 0;

	virtual CameraObject& GetCameraObject() = 0;
};