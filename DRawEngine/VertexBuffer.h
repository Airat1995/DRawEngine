#pragma once
#include <vulkan/vulkan.h>
#include <iostream>


using namespace std;

class VertexBuffer
{
public:
	VertexBuffer(const VertexBuffer&) = default;               // Copy constructor
	VertexBuffer(VertexBuffer&&) = default;                    // Move constructor
	VertexBuffer& operator=(const VertexBuffer&) = default;  // Copy assignment operator
	VertexBuffer& operator=(VertexBuffer&&) = default;       // Move assignment operator	
	virtual ~VertexBuffer() { }

	VertexBuffer(VkDevice device, VkPhysicalDevice physical, int bufferSize, int vertexCount) :_device(device), _physical(physical), _vertexCount(vertexCount)
	{
		_bufferInfo = {};
		_bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		_bufferInfo.pNext = nullptr;
		_bufferInfo.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
		_bufferInfo.size = bufferSize;
		_bufferInfo.queueFamilyIndexCount = 0;
		_bufferInfo.pQueueFamilyIndices = nullptr;
		_bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
		_bufferInfo.flags = 0;
		VkResult result = vkCreateBuffer(_device, &_bufferInfo, nullptr, &_buffer);
		if (result != VK_SUCCESS)
			cerr << "Unable to create buffer!" << endl;

		_uniqHash = reinterpret_cast<uint32_t>(this);
		
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
	}

	virtual void Fill(const void* data)
	{
		VkResult result = vkMapMemory(_device, _memory, 0, _memoryReq.size, 0, reinterpret_cast<void**>(&_dataPointer));
		if (result != VK_SUCCESS)
			cerr << "Unable to map memory!" << endl;
		memcpy(_dataPointer, data, _bufferInfo.size);
		vkUnmapMemory(_device, _memory);
	}

	virtual void Destroy()
	{
		delete _dataPointer;
		_dataPointer = nullptr;
		vkDestroyBuffer(_device, _buffer, nullptr);
	}

	VkBuffer& Buffer()
	{
		return _buffer;
	}

	int VertexCount()
	{
		return _vertexCount;
	}

	//NO NEED RIGHT NOW. CREATED TO FILL MAP
	inline bool operator<(const VertexBuffer& second) const
	{
		return _uniqHash < second._uniqHash;
	}
	
protected:

	VkBuffer _buffer;
	VkDeviceMemory _memory;
	VkDescriptorBufferInfo _bufferDescriptorInfo;
	VkDevice _device;
	VkPhysicalDevice _physical;
	VkBufferCreateInfo _bufferInfo;
	VkMemoryRequirements _memoryReq;

	int _vertexCount;

	int _uniqHash;

	uint8_t* _dataPointer = nullptr;

	vector<vec2> _data;

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
