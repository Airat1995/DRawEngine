#pragma once
#include <vector>

#include "IVertex.h"

class SkyBoxVertex : public IVertex<VertexData>
{
public:
	SkyBoxVertex(VertexData& vertexData) : _vertexData(vertexData)
	{
		SkyBoxVertex::FillVertexInfo();
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

protected:
	VertexData& _vertexData;

	vector<VertexAttributeInfo>* _vertexInfo = nullptr;

	vector<VertexBindingInfo>* _bindingInfo = nullptr;
};
