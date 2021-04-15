#pragma once
#include <vector>

#include "IVertex.h"

class BaseVertex : public IVertex<VertexData>
{
public:
	BaseVertex(VertexData& vertexData) : _vertexData(vertexData)
	{
		BaseVertex::FillVertexInfo();
	}

	void FillVertexInfo() override
	{
		_vertexInfo = new vector<VertexAttributeInfo>();
		VertexAttributeInfo position{};
		position.Location = 0;
		position.Offset = 0;
		position.Format = Format::Vector3F;
		_vertexInfo->push_back(position);

		_bindingInfo = new vector<VertexBindingInfo>();
		VertexBindingInfo firstBindingInfo{};
		firstBindingInfo.BindId = 0;
		firstBindingInfo.Size = sizeof(VertexData);
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
		return sizeof(VertexData);
	}

	VertexData& GetVertexData() override
	{
		return _vertexData;
	}

	vec4 VertexPosition() override
	{
		return vec4(_vertexData.position.x, _vertexData.position.y, _vertexData.position.z, 0.0);
	}

protected:
	VertexData& _vertexData;

	vector<VertexAttributeInfo>* _vertexInfo = nullptr;

	vector<VertexBindingInfo>* _bindingInfo = nullptr;
};
