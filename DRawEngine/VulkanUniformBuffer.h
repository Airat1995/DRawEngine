#pragma once
#include "VulkanBuffer.h"
class VulkanUniformBuffer :
	public VulkanBuffer
{
public:
	VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physical, BufferStageFlag stageFlag, BufferUsageFlag usage,
	                    BufferSharingMode sharingMode, const void* data, int size, int bindingId, bool dynamic = false);
};