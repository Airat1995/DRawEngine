#pragma once

#include <vector>
#include "IMesh.h"

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

