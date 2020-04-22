#pragma once
#include <vector>
#include "IVertex.h"

template<typename VertexDataT,
		typename = enable_if<is_base_of_v<VertexData, VertexDataT>>>
	class SimpleVertex : public IVertex<VertexDataT>
{
public:
	SimpleVertex(VertexDataT& vertexData) : _vertexData(vertexData)
	{
		SimpleVertex::FillVertexInfo();
	}
	
	void FillVertexInfo() override
	{
		_vertexInfo = new vector<VertexAttributeInfo>();
		VertexAttributeInfo firstAttributeInfo{};
		firstAttributeInfo.Location = 0;
		firstAttributeInfo.Offset = 0;
		firstAttributeInfo.Format = Format::Vector2F;
		_vertexInfo->push_back(firstAttributeInfo);

		VertexAttributeInfo secondAttributeInfo{};
		secondAttributeInfo.Location = 1;
		secondAttributeInfo.Offset = 8;
		secondAttributeInfo.Format = Format::Vector3F;
		_vertexInfo->push_back(secondAttributeInfo);

		VertexAttributeInfo thirdAttributeInfo{};
		thirdAttributeInfo.Location = 2;
		thirdAttributeInfo.Offset = 20;
		thirdAttributeInfo.Format = Format::Float;
		_vertexInfo->push_back(thirdAttributeInfo);

		_bindingInfo = new vector<VertexBindingInfo>();
		VertexBindingInfo firstBindingInfo{};
		firstBindingInfo.BindId = 0;
		firstBindingInfo.Size = 24;
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
		return 24;
	}

	VertexDataT& GetVertexData() override
	{
		return _vertexData;
	}

protected:
	VertexDataT& _vertexData;	

	vector<VertexAttributeInfo>* _vertexInfo = nullptr;

	vector<VertexBindingInfo>* _bindingInfo = nullptr;
};

template<typename VertexDataT,
	typename = enable_if<is_base_of_v<VertexData, VertexDataT>>>
	class SimpleVertex1 : public IVertex<VertexDataT>
{
public:
	SimpleVertex1(VertexDataT& vertexData) : _vertexData(vertexData)
	{
		SimpleVertex1::FillVertexInfo();
	}

	void FillVertexInfo() override
	{
		_vertexInfo = new vector<VertexAttributeInfo>();
		VertexAttributeInfo firstAttributeInfo{};
		firstAttributeInfo.Location = 0;
		firstAttributeInfo.Offset = 0;
		firstAttributeInfo.Format = Format::Vector2F;
		_vertexInfo->push_back(firstAttributeInfo);

		VertexAttributeInfo secondAttributeInfo{};
		secondAttributeInfo.Location = 1;
		secondAttributeInfo.Offset = 8;
		secondAttributeInfo.Format = Format::Vector3F;
		_vertexInfo->push_back(secondAttributeInfo);


		_bindingInfo = new vector<VertexBindingInfo>();
		VertexBindingInfo firstBindingInfo{};
		firstBindingInfo.BindId = 0;
		firstBindingInfo.Size = 20;
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
		return 20;
	}

	VertexDataT& GetVertexData() override
	{
		return _vertexData;
	}

protected:
	VertexDataT& _vertexData;

	vector<VertexAttributeInfo>* _vertexInfo = nullptr;

	vector<VertexBindingInfo>* _bindingInfo = nullptr;
};