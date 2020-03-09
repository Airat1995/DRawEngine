#pragma once
#include <vulkan/vulkan_core.h>
#include <vector>

using namespace std;

class Mesh
{
public:
	virtual vector<VkVertexInputBindingDescription>& BindingDescriptions() = 0;

	virtual vector<VkVertexInputAttributeDescription>& AttributeDescriptions() = 0;

	virtual void const* VerticesData() = 0;

	virtual int RequiredBufferSize() = 0;

	virtual int VertexSize() = 0;
};

