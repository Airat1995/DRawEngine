#pragma once

#include "IMesh.h"
#include <vector>

using namespace std;

class IRender
{
public:
	explicit IRender() = default;

	virtual ~IRender() = default;

	virtual void Init(vector<const char*>* extensions) = 0;

	virtual void AddMesh(IMesh* mesh) = 0;

	virtual void RemoveMesh(IMesh* mesh) = 0;

	virtual void DrawFrame() = 0;
};

