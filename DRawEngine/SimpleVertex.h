#pragma once
#include <vector>
#include "IVertex.h"

class SimpleVertex : public IVertex<TexturedVertexData>
{
public:
	SimpleVertex(TexturedVertexData& vertexData) : _vertexData(vertexData)
	{
		SimpleVertex::FillVertexInfo();
	}
	
	void FillVertexInfo() override
	{
		_vertexInfo = new vector<VertexAttributeInfo>();
		VertexAttributeInfo position{};
		position.Location = 0;
		position.Offset = 0;
		position.Format = Format::Vector3F;
		_vertexInfo->push_back(position);

		VertexAttributeInfo normal{};
		normal.Location = 1;
		normal.Offset = 12;
		normal.Format = Format::Vector3F;
		_vertexInfo->push_back(normal);

		VertexAttributeInfo uv{};
		uv.Location = 2;
		uv.Offset = 24;
		uv.Format = Format::Vector2F;
		_vertexInfo->push_back(uv);


		_bindingInfo = new vector<VertexBindingInfo>();
		VertexBindingInfo firstBindingInfo{};
		firstBindingInfo.BindId = 0;
		firstBindingInfo.Size = sizeof(TexturedVertexData);
		firstBindingInfo.InputRate = VK_VERTEX_INPUT_RATE_VERTEX;
		_bindingInfo->push_back(firstBindingInfo);
	}

	vector<VertexAttributeInfo>& GetVertexInfo() override
	{
		return *_vertexInfo;
	}

	vector<VertexBindingInfo>& GetBindingInfo() override
	{
		return *_bindingInfo;
	}

	int VertexSize() override
	{
		return sizeof(TexturedVertexData);
	}

	TexturedVertexData& GetVertexData() override
	{
		return _vertexData;
	}

	vec4 VertexPosition() override
	{
		return vec4(_vertexData.position.x, _vertexData.position.y, _vertexData.position.z, 0.0);
	}

protected:
	TexturedVertexData& _vertexData;

	vector<VertexAttributeInfo>* _vertexInfo = nullptr;

	vector<VertexBindingInfo>* _bindingInfo = nullptr;
};

