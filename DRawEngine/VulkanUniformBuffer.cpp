#include "VulkanUniformBuffer.h"

VulkanUniformBuffer::VulkanUniformBuffer(VkDevice device, VkPhysicalDevice physical, BufferStageFlag stageFlag,
	BufferUsageFlag usage, BufferSharingMode sharingMode, const void* data, int size, int bindingId)
	: VulkanBuffer(device, physical, stageFlag, usage, sharingMode, data, size, bindingId)
{
	VkDescriptorSetLayoutBinding uboLayoutBinding;
	uboLayoutBinding.binding = bindingId;
	uboLayoutBinding.descriptorCount = 1;
	uboLayoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	uboLayoutBinding.stageFlags = GetUsage(stageFlag);
	uboLayoutBinding.pImmutableSamplers = nullptr;

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = 1;
	layoutInfo.pBindings = &uboLayoutBinding;
	layoutInfo.flags = 0;
	layoutInfo.pNext = nullptr;

	VkResult result = vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_descriptorSetLayout);
	if(result != VK_SUCCESS)
		throw runtime_error("Unable to create descriptor set layout");
}