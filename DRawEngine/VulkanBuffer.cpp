#include "VulkanBuffer.h"

VulkanBuffer::VulkanBuffer(VkDevice device, VkPhysicalDevice physical, BufferStageFlag stageFlag, BufferUsageFlag usage, BufferSharingMode sharingMode, const void* data, int size, int bindingId) :
	_dataSize(size),
	_physical(physical),
	_usage(usage),
	_size(size),
	_bindingId(bindingId),
	_sharingMode(sharingMode),
	_layoutBinding(),
	_bufferDescriptorInfo(),
	_dataLocation(data),
	_device(device)
{
	_bufferInfo = {};
	_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	_bufferInfo.pNext = nullptr;
	_bufferInfo.usage = static_cast<VkBufferUsageFlags>(_usage);
	_bufferInfo.size = _size;	
	_bufferInfo.queueFamilyIndexCount = 0;
	_bufferInfo.pQueueFamilyIndices = nullptr;
	_bufferInfo.sharingMode = static_cast<VkSharingMode>(_sharingMode);
	_bufferInfo.flags = 0;
	

	_layoutBinding = {};
	_layoutBinding.binding = bindingId;
	_layoutBinding.descriptorCount = 1;
	_layoutBinding.descriptorType = BufferUsageToDescriptorType(usage);
	_layoutBinding.stageFlags = GetUsage(stageFlag);
	_layoutBinding.pImmutableSamplers = nullptr;

	VkResult result = vkCreateBuffer(_device, &_bufferInfo, nullptr, &_buffer);
	if (result != VK_SUCCESS)
	{
		cerr << "Unable to create buffer!" << endl;
		return;
	}

	vkGetBufferMemoryRequirements(_device, _buffer, &_memoryReq);
	VkPhysicalDeviceMemoryProperties memProperties;
	vkGetPhysicalDeviceMemoryProperties(_physical, &memProperties);

	VkMemoryAllocateInfo allocInfo;
	allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
	allocInfo.pNext = nullptr;
	allocInfo.memoryTypeIndex = FindMemoryType(_memoryReq.memoryTypeBits, memProperties,
		VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	allocInfo.allocationSize = _memoryReq.size;

	result = vkAllocateMemory(_device, &allocInfo, nullptr, &_memory);
	if (result != VK_SUCCESS)
	{
		cerr << "Unable to allocate memory!!" << endl;
		return;
	}
	result = vkBindBufferMemory(_device, _buffer, _memory, 0);
	if (result != VK_SUCCESS)
	{
		cerr << "Unable to bind memory!" << endl;
		return;
	}
	_bufferDescriptorInfo.buffer = _buffer;
	_bufferDescriptorInfo.offset = 0;
	_bufferDescriptorInfo.range = _dataSize;	
}

void VulkanBuffer::Fill()
{
	VkResult result = vkMapMemory(_device, _memory, 0, _memoryReq.size, 0, 
		reinterpret_cast<void**>(&_dataPointer));

	if (result != VK_SUCCESS)
		cerr << "Unable to map memory!" << endl;
	
	memcpy(_dataPointer, _dataLocation, _bufferInfo.size);
	vkUnmapMemory(_device, _memory);
}

void VulkanBuffer::Flush()
{
	vkFreeMemory(_device, _memory, nullptr);
	_dataPointer = nullptr;
}

VulkanBuffer::~VulkanBuffer()
{
}

void VulkanBuffer::Destroy()
{
	vkDestroyBuffer(_device, _buffer, nullptr);
	vkFreeMemory(_device, _memory, nullptr);
}

VkBuffer& VulkanBuffer::Buffer()
{
	return _buffer;
}

int VulkanBuffer::Binding() const
{
	return _bindingId;
}

int VulkanBuffer::Size() const
{
	return _layoutBinding.descriptorType == VK_DESCRIPTOR_TYPE_STORAGE_BUFFER ? 100000 : _size;
}

const void* VulkanBuffer::DataLocation()
{
	return _dataLocation;
}

VkDescriptorSetLayout VulkanBuffer::DescriptorSetLayout() const
{
	return _descriptorSetLayout;
}

VkDescriptorSetLayoutBinding VulkanBuffer::DescriptorBindingInfo() const
{
	return _layoutBinding;
}

VkDescriptorBufferInfo* VulkanBuffer::BufferDescriptorInfo()
{
	return &_bufferDescriptorInfo;
}

uint32_t VulkanBuffer::FindMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags properties)
{
	for (uint32_t i = 0; i < memoryProperties.memoryTypeCount; i++)
	{
		if ((typeFilter & (1 << i)) && (memoryProperties.memoryTypes[i].propertyFlags & properties) == properties)
		{
			return i;
		}
	}
	throw std::runtime_error("Unable to find suitable memory type!");
}

VkShaderStageFlagBits VulkanBuffer::GetUsage(BufferStageFlag stage)
{
	int usage = 0;
	if (HasFlag(stage, BufferStageFlag::Fragment))
	{
		usage |= VK_SHADER_STAGE_FRAGMENT_BIT;
	}
	if (HasFlag(stage, BufferStageFlag::Vertex))
	{
		usage |= VK_SHADER_STAGE_VERTEX_BIT;
	}
	if (HasFlag(stage, BufferStageFlag::Geometry))
	{
		usage |= VK_SHADER_STAGE_GEOMETRY_BIT;
	}
	if (HasFlag(stage, BufferStageFlag::TesselationControl))
	{
		usage |= VK_SHADER_STAGE_TESSELLATION_CONTROL_BIT;
	}
	if (HasFlag(stage, BufferStageFlag::TesselationEvaluation))
	{
		usage |= VK_SHADER_STAGE_TESSELLATION_EVALUATION_BIT;
	}

	return static_cast<VkShaderStageFlagBits>(usage);
}

VkDescriptorType VulkanBuffer::BufferUsageToDescriptorType(BufferUsageFlag bufferUsageFlag)
{
	VkDescriptorType descriptor;
	switch (bufferUsageFlag)
	{
	case BufferUsageFlag::UniformTexel: descriptor = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER; break;
	case BufferUsageFlag::StorageTexel: descriptor = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER; break;
	case BufferUsageFlag::UniformBuffer: descriptor = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
	case BufferUsageFlag::StorageBuffer: descriptor = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; break;
	case BufferUsageFlag::TransferSrc: descriptor = VK_DESCRIPTOR_TYPE_SAMPLED_IMAGE; break;
	default:
		descriptor = (VkDescriptorType)bufferUsageFlag;
		cerr << "Current type not supported! Please add new case statement in VulkanPipeline->BufferUsageToDescriptorType" << endl;
		break;
	}

	return descriptor;
}

