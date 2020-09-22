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
		position = vec3(0, 0, 0);
	}

	vec3 position;
};

class TexturedVertexData : public VertexData
{
public:
	TexturedVertexData(const TexturedVertexData&) = default;
	TexturedVertexData(TexturedVertexData&&) = default;
	TexturedVertexData& operator=(const TexturedVertexData&) = default;
	TexturedVertexData& operator=(TexturedVertexData&&) = default;
	explicit TexturedVertexData()
	{
		position = vec3(0, 0, 0);
		normal = vec3(0, 0, 0);
		uv = vec2(0, 0);
	}
	vec3 normal;
	vec2 uv;
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
		position = vec3(0, 0, 0);
		color = vec3(0, 0, 0);
		texCoord = vec2(0, 0);
	}

	vec2 texCoord;
	vec3 color;
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

