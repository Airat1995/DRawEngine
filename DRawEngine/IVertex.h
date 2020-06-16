#pragma once
#include <glm/glm.hpp>
#include "VertexAttributeInfo.h"
#include "VertexBindingInfo.h"

using namespace std;
using namespace glm;


class VertexData
{
public:
	VertexData(const VertexData&) = default;
	VertexData(VertexData&&) = default;
	VertexData& operator=(const VertexData&) = default;
	VertexData& operator=(VertexData&&) = default;
	explicit VertexData()
	{
		position = glm::vec3(0, 0, 0);
	}

	glm::vec3 position;
};

class ColoredVertexData : public VertexData
{
public:

	ColoredVertexData(const ColoredVertexData&) = default;
	ColoredVertexData(ColoredVertexData&&) = default;
	ColoredVertexData& operator=(const ColoredVertexData&) = default;
	ColoredVertexData& operator=(ColoredVertexData&&) = default;
	explicit ColoredVertexData()
	{
		position = glm::vec3(0, 0, 0);
		color = glm::vec3(0, 0, 0);
		texCoord = glm::vec2(0, 0);
	}

	glm::vec2 texCoord;
	glm::vec3 color;
};

class AlphaColoredVertexData : public ColoredVertexData
{
public:
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

