#pragma once
#include <vulkan/vulkan.h>
#include <exception>

#include "ICommandPool.h"


class ICommandBuffer
{
public:
	ICommandBuffer(VkDevice* device, VkCommandPool* commandPool);

	VkCommandBuffer* CommandBuffer();

	virtual ~ICommandBuffer();
private:
	VkCommandBuffer _commandBuffer;
};

