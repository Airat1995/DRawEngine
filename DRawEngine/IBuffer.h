#pragma once
#include <iostream>

#include "SwapchainBuffer.h"

using namespace std;

enum class SharingMode
{
	Exclusive = 0,
	Concurrent = 1,
};

enum class BufferUsageFlag
{
	TransferSrc = 0x00000001,
	TransferDst = 0x00000002,
	UniformTexel = 0x00000004,
	StorageTexel = 0x00000008,
	UniformBuffer = 0x00000010,
	StorageBuffer = 0x00000020,
	IndexBuffer = 0x00000040,
	VertexBuffer = 0x00000080,
	IndirectBuffer = 0x00000100,
	TransformFeedbackBuffer = 0x00000800,
	TransformFeedbackCounter = 0x00001000,
	ConditionalRendering = 0x00000200,
	RayTracingNV = 0x00000400,
	ShaderDeviceAddress = 0x00020000
};

template<class T>
class IBuffer
{
public:
	IBuffer(VkDevice* device, VkPhysicalDevice physical, BufferUsageFlag usage, SharingMode sharingMode, T* data):
		_bufferInfo(),
		_device(device),
		_data(data)
	{
		VkBufferCreateInfo buf_info = {};
		buf_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		buf_info.pNext = nullptr;
		buf_info.usage = static_cast<VkBufferUsageFlags>(usage);
		buf_info.size = sizeof(T);
		buf_info.queueFamilyIndexCount = 0;
		buf_info.pQueueFamilyIndices = nullptr;
		buf_info.sharingMode = static_cast<VkSharingMode>(sharingMode);
		buf_info.flags = 0;
		VkResult result = vkCreateBuffer(*device, &buf_info, nullptr, &_buffer);
		if (result != VK_SUCCESS)
			cerr << "Unable to create buffer!" << endl;

		vkGetBufferMemoryRequirements(*device, _buffer, &_memoryReq);
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physical, &memProperties);

		VkMemoryAllocateInfo allocInfo;
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.pNext = nullptr;
		allocInfo.memoryTypeIndex = FindMemoryType(_memoryReq.memoryTypeBits, memProperties,
		                                           VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
		                                           VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
		allocInfo.allocationSize = _memoryReq.size;

		result = vkAllocateMemory(*device, &allocInfo, nullptr, &_memory);
		if (result != VK_SUCCESS)
		{
			cerr << "Unable to allocate memory!!" << endl;
			return;
		}
		result = vkBindBufferMemory(*device, _buffer, _memory, 0);
		if (result != VK_SUCCESS)
		{
			cerr << "Unable to bind memory!" << endl;
			return;
		}
	}

	virtual void Fill()
	{
		VkResult result = vkMapMemory(*_device, _memory, 0, _memoryReq.size, 0, reinterpret_cast<void**>(&_dataPointer));
		if (result != VK_SUCCESS)
			cerr << "Unable to map memory!" << endl;
		memcpy(_dataPointer, _data, sizeof(T));
		vkUnmapMemory(*_device, _memory);

		result = vkBindBufferMemory(*_device, _buffer, _memory, 0);
		if (result != VK_SUCCESS)
			cerr << "Unable to bind buffer memory!" << endl;

		_bufferInfo.buffer = _buffer;
		_bufferInfo.offset = 0;
		_bufferInfo.range = sizeof(T);
	}

	virtual void Flush()
	{
		vkFreeMemory(*_device, _memory, nullptr);
		_dataPointer = nullptr;
	}

	virtual ~IBuffer()
	{
		vkDestroyBuffer(*_device, _buffer, nullptr);
		vkFreeMemory(*_device, _memory, nullptr);
	}

protected:
	VkBuffer _buffer;
	VkDeviceMemory _memory;
	VkDescriptorBufferInfo _bufferInfo;
	VkDevice* _device;
	VkMemoryRequirements _memoryReq;
	
	uint8_t* _dataPointer = nullptr;
	T* _data;

	static uint32_t FindMemoryType(uint32_t typeFilter, VkPhysicalDeviceMemoryProperties memoryProperties, VkMemoryPropertyFlags properties)
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
};