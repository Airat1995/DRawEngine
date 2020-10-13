#pragma once
#include <glm/vec3.hpp>

using namespace glm;

class PointLight
{
public:
	alignas(16) vec3 position;
	alignas(16) vec3 color;
	alignas(16) vec3 cameraPosition;
	alignas(4) float power;
};
