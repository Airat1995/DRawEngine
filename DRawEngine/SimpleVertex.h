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

protected:
	TexturedVertexData& _vertexData;

	vector<VertexAttributeInfo>* _vertexInfo = nullptr;

	vector<VertexBindingInfo>* _bindingInfo = nullptr;
};

template<typename VertexDataT,
	typename = enable_if<is_base_of_v<VertexData, VertexDataT>>>
	class SimpleVertex1 : public IVertex<VertexDataT>
{
public:

	SimpleVertex1(const SimpleVertex1&) = default;
	SimpleVertex1(SimpleVertex1&&) = default;
	SimpleVertex1& operator=(const SimpleVertex1&) = default;
	SimpleVertex1& operator=(SimpleVertex1&&) = default;

	SimpleVertex1(VertexDataT vertexData) : _vertexData(vertexData)
	{
		SimpleVertex1::FillVertexInfo();
	}

	void FillVertexInfo() override
	{
		_vertexInfo = new vector<VertexAttributeInfo>();
		VertexAttributeInfo firstAttributeInfo{};
		firstAttributeInfo.Location = 0;
		firstAttributeInfo.Offset = 0;
		firstAttributeInfo.Format = Format::Vector3F;
		_vertexInfo->push_back(firstAttributeInfo);

		VertexAttributeInfo texCoordAttributeInfo{};
		texCoordAttributeInfo.Location = 1;
		texCoordAttributeInfo.Offset = 12;
		texCoordAttributeInfo.Format = Format::Vector2F;
		_vertexInfo->push_back(texCoordAttributeInfo);

		VertexAttributeInfo secondAttributeInfo{};
		secondAttributeInfo.Location = 2;
		secondAttributeInfo.Offset = 20;
		secondAttributeInfo.Format = Format::Vector3F;
		_vertexInfo->push_back(secondAttributeInfo);


		_bindingInfo = new vector<VertexBindingInfo>();
		VertexBindingInfo firstBindingInfo{};
		firstBindingInfo.BindId = 0;
		firstBindingInfo.Size = sizeof(VertexDataT);
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
		return sizeof(VertexDataT);
	}

	VertexDataT& GetVertexData() override
	{
		return _vertexData;
	}

protected:
	VertexDataT _vertexData;

	vector<VertexAttributeInfo>* _vertexInfo = nullptr;

	vector<VertexBindingInfo>* _bindingInfo = nullptr;

};