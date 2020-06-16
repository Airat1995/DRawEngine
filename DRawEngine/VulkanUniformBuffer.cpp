#include "VulkanUniformBuffer.h"

VulkanUniformBuffer::VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physical, BufferStageFlag stageFlag,
	BufferUsageFlag usage, BufferSharingMode sharingMode, const void* data, int size, int bindingId)
	: VulkanBuffer(device, physical, stageFlag, usage, sharingMode, data, size, bindingId)
{
	_layoutBinding = {};
	_layoutBinding.binding = bindingId;
	_layoutBinding.descriptorCount = 1;
	_layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	_layoutBinding.stageFlags = GetUsage(stageFlag);
	_layoutBinding.pImmutableSamplers = nullptr;
}