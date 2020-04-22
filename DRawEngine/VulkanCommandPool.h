#pragma once

#include <vector>
#include <exception>
#include <vulkan/vulkan.h>
#include "ICommandPool.h"
#include "VulkanCommandBuffer.h"

using namespace std;

class VulkanCommandPool : public ICommandPool
{
public:
	VulkanCommandPool(VkDevice device, int queueFamilyIndex);
	
	virtual ~VulkanCommandPool();

	void AddCommandBuffer();

	void BeginCommandPool();

	int CommandBufferCount();

	VulkanCommandBuffer& CommandBuffer(int index);

	void ResetCommandBuffers();

	VkCommandPool CommandPool();

	VkCommandBuffer* CommandBuffersData();

private:
	VkCommandPool _commandPool;

	VkDevice _device;

	std::vector<VulkanCommandBuffer> _commandBuffers;
};

