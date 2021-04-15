#pragma once
#include <glm/glm.hpp>

using namespace glm;

class ShadowmapBuffer
{
public:
	mat4 ModelMatrix;
	mat4 ViewMatrix;
	mat4 ProjectionMatrix;
	vec4 LightPosition;
};
