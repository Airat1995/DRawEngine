#pragma once
#include <glm/glm.hpp>
#include "VertexAttributeInfo.h"
#include "VertexBindingInfo.h"

using namespace std;
using namespace glm;


struct VertexData
{
	glm::vec2 position;
};

struct ColoredVertexData : VertexData
{
	glm::vec3 color;
};

struct AlphaColoredVertexData : ColoredVertexData
{
	float alpha;
};


/**
 * Base data description structure
 * MUST FIT _bindingDescriptions and _attributeDescriptions to properly work with vertex input buffer
 */
template<typename  T>
class IVertex
{
public:
	virtual ~IVertex() = default;
	
	virtual void FillVertexInfo() = 0;

	virtual vector<VertexAttributeInfo>& GetVertexInfo() = 0;

	virtual vector<VertexBindingInfo>& GetBindingInfo() = 0;

	virtual int VertexSize() = 0;

	virtual T& GetVertexData() = 0;	
};

