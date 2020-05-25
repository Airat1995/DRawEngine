#pragma once
#include "IMesh.h"
#include "IVertex.h"

using namespace std;

template
<
	typename Vertex,
	typename VertexDataT,
	typename = enable_if<is_base_of_v<VertexData, VertexDataT>>,
	typename = enable_if<is_base_of_v<IVertex<VertexDataT>, Vertex>>
>
class Mesh : public IMesh
{
public:
	Mesh(const Mesh&) = default;               // Copy constructor
	Mesh(Mesh&&) = default;                    // Move constructor
	Mesh& operator=(const Mesh&) = delete;  // Copy assignment operator
	Mesh& operator=(Mesh&&) = delete;       // Move assignment operator
	virtual ~Mesh() = default;

	Mesh(vector<Vertex>& vertices, IMaterial& material) : IMesh(), _vertices(vertices), _material(material)
	{
		vertices.at(0).FillVertexInfo();
		FillVertexDatas();
	}

	Mesh(): IMesh()
	{
		_vertices = vector<Vertex>();		
	}

	Mesh(vector<Vertex>& vertices, vector<uint16_t> indexes, IMaterial& material) : IMesh(), _vertices(vertices),
	                                                            _indexes(indexes), _material(material)
	{
		_indexed = true;
		vertices.at(0).FillVertexInfo();
		FillVertexDatas();
	}

	void FillVertexDatas()
	{
		_vertexDatas = vector<VertexDataT>();
		for (size_t index = 0; index < _vertices.size(); ++index)
		{
			_vertexDatas.push_back(_vertices[index].GetVertexData());
		}
	}

	const void* VerticesData() override
	{
		return _vertexDatas.data();
	}

	int VertexCount() override
	{
		return _vertices.size();
	}

	void const* IndicesData() override
	{
		return _indexes.data();
	}

	int RequiredBufferSize() override
	{
		return _vertices[0].VertexSize() * _vertexDatas.size();
	}

	int VertexSize() override
	{
		return sizeof(Vertex);
	}

	int IndexSize() override
	{
		return _indexes.size();
	}

	vector<VertexAttributeInfo> VertexInfo() override
	{
		return _vertices.at(0).GetVertexInfo();
	}

	vector<VertexBindingInfo> GetVertexBindingInfo() override
	{
		return _vertices.at(0).GetBindingInfo();
	}

	map<ShaderType, IShader>& Shaders() override
	{
		return _material.MaterialShaders();
	}

	IMaterial& Material() override
	{
		return _material;
	}
protected:

	vector<Vertex> _vertices;

	vector<uint16_t> _indexes;

private:
	bool _indexed = false;

	vector<VertexDataT> _vertexDatas;

	IMaterial _material;
};

