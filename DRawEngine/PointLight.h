#pragma once
#include <glm/vec3.hpp>

using namespace glm;
const int LIGHTS_ITEMS_COUNT = 3;

class PointLight
{
public:
	vec4 position;
	vec4 color;
	vec4 lightContsts;
};


class LightsContainers
{
public:
	vec4 cameraPosition = vec4(0);
	PointLight pointLights[LIGHTS_ITEMS_COUNT];
};