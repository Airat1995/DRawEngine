#pragma once
#include "IMesh.h"
#include "VulkanShader.h"
#include "VulkanBuffer.h"
#include "VulkanImage.h"
#include "IVulkanRenderMeshBufferCreator.h"


using namespace std;

class VulkanMeshData
{
public:
	VulkanMeshData(IMesh* mesh, vector<VulkanBuffer>& buffers, vector<VulkanImage>& images,
		vector<VulkanBuffer>& perObjectBuffers);

	vector<VkVertexInputBindingDescription> BindingDescriptions();

	vector<VkVertexInputAttributeDescription> AttributeDescriptions();

	void AddMesh(IMesh* mesh, vector<VulkanBuffer>& vulkanBuffers);

	vector<IMesh*>& Meshes();

	vector<VulkanBuffer>& Buffers();

	vector<VulkanImage>& Images();

	vector<VulkanBuffer>& PerObjectBuffersInfo();

	vector<VulkanBuffer>& PerObjectBuffersInfo(IMesh* mesh);

	void SetBufferRecreateEventListener(IVulkanRenderMeshBufferCreator* bufferCreator);
	bool ShouldCombine(IMesh* mesh);

private:
	vector<VkVertexInputBindingDescription> _bindingDescriptions;

	vector<VkVertexInputAttributeDescription> _attributeDescriptions;

	vector<IMesh*> _meshes;

	vector<VulkanBuffer>& _buffers;

	vector<VulkanImage>& _images;

	map<IMesh*, vector<VulkanBuffer>>& _perObjectBuffers = map<IMesh*, vector<VulkanBuffer>>();

	IVulkanRenderMeshBufferCreator* _bufferCreator = nullptr;

	bool _needRebuild;
	
	inline VkVertexInputAttributeDescription CreateAttributeDescription(
		uint32_t binding,
		uint32_t location,
		VkFormat format,
		uint32_t offset)
	{
		VkVertexInputAttributeDescription vInputAttribDescription{};
		vInputAttribDescription.location = location;
		vInputAttribDescription.binding = binding;
		vInputAttribDescription.format = format;
		vInputAttribDescription.offset = offset;
		return vInputAttribDescription;
	}

	inline VkVertexInputBindingDescription CreateInputBindingDescription(
		uint32_t binding,
		uint32_t stride,
		VkVertexInputRate inputRate)
	{
		VkVertexInputBindingDescription vInputBindDescription{};
		vInputBindDescription.binding = binding;
		vInputBindDescription.stride = stride;
		vInputBindDescription.inputRate = inputRate;
		return vInputBindDescription;
	}

protected:
	inline VkFormat EnumFormatToVulkanFormat(Format format)
	{
		VkFormat vulkanFormat = VK_FORMAT_UNDEFINED;
		switch (format)
		{
		case Format::Int:
			vulkanFormat = VK_FORMAT_R8_SINT;
			break;
		case Format::UInt:
			vulkanFormat = VK_FORMAT_R8_UINT;
			break;
		case Format::Float:
			vulkanFormat = VK_FORMAT_R32_SFLOAT;
			break;
		case Format::Double:
			vulkanFormat = VK_FORMAT_R16_SFLOAT;
			break;
		case Format::Vector1F:
		case Format::Vector1SF:
			vulkanFormat = VK_FORMAT_R8_SSCALED;
			break;
		case Format::Vector1I:
			vulkanFormat = VK_FORMAT_R8_SSCALED;
			break;
		case Format::Vector2F:
		case Format::Vector2SF:
			vulkanFormat = VK_FORMAT_R32G32_SFLOAT;
			break;
		case Format::Vector2I:
			vulkanFormat = VK_FORMAT_R32G32_SINT;
			break;
		case Format::Vector3F:
		case Format::Vector3SF:
			vulkanFormat = VK_FORMAT_R32G32B32_SFLOAT;
			break;
		case Format::Vector3I:
			vulkanFormat = VK_FORMAT_R32G32B32_SINT;
			break;
		case Format::Vector4F:
		case Format::Vector4SF:
			vulkanFormat = VK_FORMAT_R32G32B32A32_SFLOAT;
			break;	
		case Format::Vector4I:
			vulkanFormat = VK_FORMAT_R32G32B32A32_SINT;
			break;
		case Format::Quaternion:
			break;
		default:;
		}

		return vulkanFormat;
	}
};