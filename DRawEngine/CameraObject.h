#pragma once
#include <glm/glm.hpp>

using namespace glm;

struct CameraObject {
	mat4 _model = mat4(
		1, 0, 0, 0,
		0, 1, 0, 1,
		0, 0, 1, 1,
		0, 0, 0, 1
	);
	mat4 _view;
	mat4 _proj;
};

