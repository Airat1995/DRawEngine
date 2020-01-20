#include "ICommandPool.h"

ICommandPool::ICommandPool(VkDevice* device, int queueFamilyIndex)
{
	VkCommandPoolCreateInfo commandPoolInfo;
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolInfo.flags = 0;

	const VkResult result = vkCreateCommandPool(*device, &commandPoolInfo, nullptr, &_commandPool);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create command pool!");
	}
}

ICommandPool::~ICommandPool()
{	
	vkFreeCommandBuffers(*_device, _commandPool, _commandBuffers.size(), _commandBuffers.data());
	vkDestroyCommandPool(*_device, _commandPool, nullptr);

}

void ICommandPool::AddCommandBuffer()
{
	ICommandBuffer commandBuffer = ICommandBuffer(_device, &_commandPool);
	_commandBuffers.push_back(*commandBuffer.CommandBuffer());
}

VkCommandPool* ICommandPool::CommandPool()
{
	return &_commandPool;
}
