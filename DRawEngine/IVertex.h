#pragma once

/**
 * Base data description structure
 * MUST FIT _bindingDescriptions and _attributeDescriptions to properly work with vertex input buffer
 */
class IVertex
{
public:
	IVertex()
	{
	}
	
	static vector<VkVertexInputBindingDescription>& BindingDescriptions()
	{
		return _bindingDescriptions;
	}

	static vector<VkVertexInputAttributeDescription>& AttributeDescriptions()
	{
		return _attributeDescriptions;
	}

protected:
	inline static vector<VkVertexInputBindingDescription> _bindingDescriptions = vector<VkVertexInputBindingDescription>();

	inline static vector<VkVertexInputAttributeDescription> _attributeDescriptions = vector<VkVertexInputAttributeDescription>();
};

