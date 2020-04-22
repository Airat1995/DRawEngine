#pragma once
#include <vulkan/vulkan.h>

struct VertexBindingInfo
{
	VkVertexInputRate InputRate;

	int Size;

	int BindId;
};

