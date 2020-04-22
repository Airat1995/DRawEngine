#pragma once

#include "Mesh.h"
#include <vector>

using namespace std;

class IRender
{
public:
	explicit IRender() = default;

	virtual ~IRender() = default;

	virtual void Init(vector<const char*>* extensions) = 0;

	virtual void AddMesh(Mesh* mesh) = 0;

	virtual void RemoveMesh(Mesh* mesh) = 0;

	virtual void DrawFrame() = 0;
};

