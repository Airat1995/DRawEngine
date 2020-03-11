#pragma once
#include <vector>
#include "Mesh.h"

using namespace std;

template<typename Vertex,
		 typename = enable_if<is_base_of_v<IVertex, Vertex>>>
class IMesh : public Mesh
{
public:
	IMesh(vector<Vertex>& vertices) : _vertices(vertices)
	{
	}

	IMesh()
	{
		_vertices = vector<Vertex>();
	}

	IMesh(vector<Vertex>& vertices, vector<uint16_t> indexes) : _vertices(vertices), _indexes(indexes)
	{
		_indexed = true;
	}

	void const* VerticesData()
	{
		return _vertices.data();
	}

	void const* IndicesData() override
	{
		return _indexes.data();
	}

	int RequiredBufferSize() override
	{
		return sizeof(Vertex) * _vertices.size();
	}

	int VertexSize() override
	{
		return sizeof(Vertex);
	}

	int IndexSize() override
	{
		return _indexes.size();
	}

	vector<VkVertexInputBindingDescription>& BindingDescriptions() override
	{
		return  Vertex::BindingDescriptions();
	}

	vector<VkVertexInputAttributeDescription>& AttributeDescriptions() override
	{
		return Vertex::AttributeDescriptions();
	}

protected:

	vector<Vertex> _vertices;

	vector<uint16_t> _indexes;

private:
	bool _indexed = false;
};

