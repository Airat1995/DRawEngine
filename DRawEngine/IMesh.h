#pragma once
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

#include "IShader.h"
#include "VertexAttributeInfo.h"
#include "VertexBindingInfo.h"
#include "IMaterial.h"

using namespace std;
using namespace glm;

class IMesh
{
public:
			
	virtual const void* VerticesData() = 0;

	virtual void const* IndicesData() = 0;

	virtual int VertexCount() = 0;

	virtual int RequiredBufferSize() = 0;

	virtual int VertexSize() = 0;

	virtual int IndexSize() = 0;

	virtual vector<VertexAttributeInfo> VertexInfo() = 0;
	
	virtual vector<VertexBindingInfo> GetVertexBindingInfo() = 0;	

	virtual map<ShaderType, IShader>& Shaders() = 0;

	virtual IMaterial& Material() = 0;
	
protected:
	vec3 position;
	quat rotation;
};
