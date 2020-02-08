#include "ICommandPool.h"

ICommandPool::ICommandPool(VkDevice device, int queueFamilyIndex) : _device(device)
{
	VkCommandPoolCreateInfo commandPoolInfo;
	commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	commandPoolInfo.pNext = nullptr;
	commandPoolInfo.queueFamilyIndex = queueFamilyIndex;
	commandPoolInfo.flags = 0;

	const VkResult result = vkCreateCommandPool(device, &commandPoolInfo, nullptr, &_commandPool);
	if (result != VK_SUCCESS)
	{
		throw exception("Unable to create command pool!");
	}
}

ICommandPool::~ICommandPool()
{	
	vector<VkCommandBuffer> commandBuffers = vector<VkCommandBuffer>(_commandBuffers.size());
	for (auto& _commandBuffer : _commandBuffers)
		commandBuffers.push_back(*_commandBuffer.CommandBuffer());

	vkFreeCommandBuffers(_device, _commandPool, commandBuffers.size(), commandBuffers.data());
	vkDestroyCommandPool(_device, _commandPool, nullptr);

}

void ICommandPool::AddCommandBuffer()
{
	const ICommandBuffer commandBuffer = ICommandBuffer(_device, &_commandPool);
	_commandBuffers.push_back(commandBuffer);
}

void ICommandPool::BeginCommandPool()
{
	for (auto commandBuffer : _commandBuffers)
		commandBuffer.BeginCommandBuffer();
}

VkCommandPool* ICommandPool::CommandPool()
{
	return &_commandPool;
}
