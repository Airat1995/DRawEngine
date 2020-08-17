#pragma once
#include <vulkan/vulkan.h>
#include <exception>
#include <stdexcept>
#include "ICommandBuffer.h"


class VulkanCommandBuffer : public ICommandBuffer
{
public:
	VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool);

	VkCommandBuffer CommandBuffer();

	virtual ~VulkanCommandBuffer();
	
	bool BeginCommandBuffer();

	void EndCommandBuffer();

	void Reset();
private:
	VkCommandBuffer _commandBuffer;
};

