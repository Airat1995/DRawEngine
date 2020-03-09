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

	void const* VerticesData()
	{
		return _vertices.data();
	}

	int RequiredBufferSize() override
	{
		return sizeof(Vertex) * _vertices.size();
	}

	int VertexSize() override
	{
		return sizeof(Vertex);
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
};

