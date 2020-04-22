#pragma once
#include "Mesh.h"
#include "IVertex.h"

using namespace std;

template
<
	typename Vertex,
	typename VertexDataT,
	typename = enable_if<is_base_of_v<VertexData, VertexDataT>>,
	typename = enable_if<is_base_of_v<IVertex<VertexDataT>, Vertex>>
>
class IMesh : public Mesh
{
public:
	IMesh(const IMesh&) = default;               // Copy constructor
	IMesh(IMesh&&) = default;                    // Move constructor
	IMesh& operator=(const IMesh&) = delete;  // Copy assignment operator
	IMesh& operator=(IMesh&&) = delete;       // Move assignment operator
	virtual ~IMesh() = default;

	IMesh(vector<Vertex>& vertices, vector<IShader>& shaders) : Mesh(), _vertices(vertices)
	{
		_shaders = shaders;
		vertices.at(0).FillVertexInfo();
		FillVertexDatas();
	}

	IMesh(): Mesh()
	{
		_vertices = vector<Vertex>();		
	}

	IMesh(vector<Vertex>& vertices, vector<uint16_t> indexes) : Mesh(), _vertices(vertices),
	                                                            _indexes(indexes)
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

	vector<IShader>& Shaders() override
	{
		return _shaders;
	}

protected:

	vector<Vertex> _vertices;

	vector<uint16_t> _indexes;

private:
	bool _indexed = false;

	vector<VertexDataT> _vertexDatas;
};

