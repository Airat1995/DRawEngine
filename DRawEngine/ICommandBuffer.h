#pragma once
#include <vulkan/vulkan.h>
#include <exception>
#include <stdexcept>


class ICommandBuffer
{
public:
	ICommandBuffer(VkDevice* device, VkCommandPool* commandPool);

	VkCommandBuffer* CommandBuffer();

	virtual ~ICommandBuffer();
	
	bool BeginCommandBuffer();
private:
	VkCommandBuffer _commandBuffer;
};

