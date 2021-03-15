#pragma once
#include <glm/vec3.hpp>

using namespace glm;

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
	int colorsCount = 0;
	vector<PointLight> lightObject;
};