#pragma once
#include <glm/vec4.hpp>

using namespace glm;

class MaterialParameter
{
public:
	vec4 ambient;
	vec4 diffuse;
	vec4 specular;
	mat4 model;
};
