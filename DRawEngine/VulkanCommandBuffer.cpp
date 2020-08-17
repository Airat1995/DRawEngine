#include "VulkanCommandBuffer.h"


VulkanCommandBuffer::VulkanCommandBuffer(VkDevice device, VkCommandPool commandPool)
{
	VkCommandBufferAllocateInfo allocateInfo;
	allocateInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocateInfo.pNext = nullptr;
	allocateInfo.commandPool = commandPool;
	allocateInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocateInfo.commandBufferCount = 1;

	const VkResult result = vkAllocateCommandBuffers(device, &allocateInfo, &_commandBuffer);
	if (result != VK_SUCCESS)
	{
		throw std::exception("Unable to create command buffer!");
	}

}

VkCommandBuffer VulkanCommandBuffer::CommandBuffer()
{
	return _commandBuffer;
}

VulkanCommandBuffer::~VulkanCommandBuffer()
{
	//Command buffer getting destroyed by ICommandPool
	//Created like to decrease Vulkan send commands
}

bool VulkanCommandBuffer::BeginCommandBuffer()
{
	VkCommandBufferBeginInfo beginInfo;
	beginInfo.pNext = nullptr;
	beginInfo.flags = 0;
	beginInfo.pInheritanceInfo = nullptr;
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	VkResult result = vkBeginCommandBuffer(_commandBuffer, &beginInfo);
	if (result != VK_SUCCESS)
		throw std::runtime_error("Unable to start command buffer!");
	return true;
}

void VulkanCommandBuffer::EndCommandBuffer()
{
	if (vkEndCommandBuffer(_commandBuffer) != VK_SUCCESS)
		throw std::runtime_error("failed to record command buffer!");
}

void VulkanCommandBuffer::Reset()
{
	if (vkResetCommandBuffer(_commandBuffer, VkCommandBufferResetFlagBits::VK_COMMAND_BUFFER_RESET_RELEASE_RESOURCES_BIT) != VK_SUCCESS)
		throw std::runtime_error("failed to reset command buffer!");
}
