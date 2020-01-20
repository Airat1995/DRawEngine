#include "ICommandBuffer.h"


ICommandBuffer::ICommandBuffer(VkDevice* device, VkCommandPool* commandPool)
{
	VkCommandBufferAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.commandPool = *commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = 1;

	const VkResult result = vkAllocateCommandBuffers(*device, &allocateInfo, &_commandBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::exception("Unable to create command buffer!");
	}

}

VkCommandBuffer* ICommandBuffer::CommandBuffer()
{
	return &_commandBuffer;
}

ICommandBuffer::~ICommandBuffer()
{
	//Command buffer getting destroyed by ICommandPool
	//Created like to decrease Vulkan send commands
}
