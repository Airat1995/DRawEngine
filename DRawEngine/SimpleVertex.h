#pragma once
#include "IVertex.h"
#include <glm/glm.hpp>
#include <vulkan/vulkan_core.h>
#include <vector>

class SimpleVertex : public IVertex
{
	friend class IVertex;
public:
	SimpleVertex(glm::vec2& position) : IVertex()
	{
		_position = position;
		if (_bindingDescriptions.empty())
		{
			VkVertexInputBindingDescription bindingDescription = {};
			bindingDescription.binding = 0;
			bindingDescription.stride = sizeof(SimpleVertex);
			bindingDescription.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
			_bindingDescriptions.push_back(bindingDescription);
		}
		if (_attributeDescriptions.empty())
		{
			VkVertexInputAttributeDescription attributeDescription = {};
			attributeDescription.binding = 0;
			attributeDescription.location = 0;
			attributeDescription.format = VK_FORMAT_R32G32_SFLOAT;
			attributeDescription.offset = offsetof(SimpleVertex, _position);
			_attributeDescriptions.push_back(attributeDescription);
		}
	}

protected:
	glm::vec2 _position;
};
