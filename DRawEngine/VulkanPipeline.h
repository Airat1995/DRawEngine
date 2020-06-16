#pragma once

#include "IPipeline.h"
#include "VulkanShader.h"
#include "VulkanRenderpass.h"
#include "VulkanMeshData.h"

using namespace std;

class VulkanPipeline : public IPipeline
{
public:
	explicit VulkanPipeline(VkDevice device, VkPhysicalDevice physical, VulkanRenderpass& renderpass,
		VulkanMeshData& vulkanMeshData, VkFormat imageFormat, VkExtent2D extent);

	void Initialize(VkDevice device, VulkanMeshData& vulkanMeshData,
		VkFormat imageFormat, VkExtent2D extent);

	VkPipeline Pipeline();

	virtual ~VulkanPipeline();

	void DrawFrame(VkCommandBuffer commandBuffer);

	void DestroyPipeline();

private:

	void CreateBuffers(VulkanMeshData& meshData);
	
	vector<VulkanShader> BaseShadersToVulkanShader(VkDevice device, map<ShaderType, IShader>& shaders);

	VkDescriptorType BufferUsageToDescriptorType(BufferUsageFlag bufferUsageFlag);
	
	VkDynamicState _dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineLayout _pipelineLayout{};

	VkDevice _device{};

	VkPhysicalDevice _physical;
	
	VkPipeline _pipeline{};

	VkDescriptorPool _descriptorPool;

	VkDescriptorSet _descriptorSets;

	VkDescriptorSetLayout _descriptorSetLayout;

	VulkanRenderpass& _renderPass;

	VulkanMeshData& _meshData;

	vector<VertexBuffer> _meshBuffers;

	vector<VulkanBuffer> _dataBuffers;

	uint32_t _firstBinding;

	size_t _bindingCount;
};
