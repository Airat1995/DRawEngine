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
		vector<VulkanMeshData>& vulkanMeshData, VkFormat imageFormat, VkExtent2D extent);

	void Initialize(VkDevice device, vector<VulkanMeshData>& vulkanMeshData,
		VkFormat imageFormat, VkExtent2D extent);

	VkPipeline Pipeline();

	virtual ~VulkanPipeline();

	void DrawFrame(VkCommandBuffer commandBuffer);

	void DestroyPipeline();

private:

	void CreateBuffers(vector<VulkanMeshData>& meshData);

	vector<VulkanShader> BaseShadersToVulkanShader(VkDevice device, vector<IShader>& shaders);
	
	VkDynamicState _dynamicStates[2] = {
		VK_DYNAMIC_STATE_VIEWPORT,
		VK_DYNAMIC_STATE_LINE_WIDTH
	};

	VkPipelineLayout _pipelineLayout{};

	VkDevice _device{};

	VkPhysicalDevice _physical;
	
	VkPipeline _pipeline{};

	VulkanRenderpass& _renderPass;

	vector<VulkanMeshData>& _meshData;

	vector<VertexBuffer> _meshBuffers;

	uint32_t _firstBinding;

	size_t _bindingCount;
};
