#pragma once
#include "IPipeline.h"
#include "IVulkanRenderMeshBufferCreator.h"
#include "VulkanShader.h"
#include "VulkanRenderpass.h"
#include "VulkanMeshData.h"
#include "VertexBuffer.h"

using namespace std;

class VulkanPipeline : public IPipeline, public IVulkanRenderMeshBufferCreator
{
public:
	explicit VulkanPipeline(VkDevice device, VkPhysicalDevice physical, VulkanRenderpass& renderpass,
		VulkanMeshData& vulkanMeshData, VkExtent2D extent);

	virtual ~VulkanPipeline();

	void Initialize(VkDevice device, VulkanMeshData& vulkanMeshData, VkExtent2D extent);

	VkPipeline Pipeline();

	VkPipelineLayout PipelineLayout();

	void DestroyPipeline();
	
	void BindBuffer(VkCommandBuffer commandBuffer);

	void BindPipeline(VkCommandBuffer commandBuffer);

	void BuildCommandbuffer(VkCommandBuffer commandBuffer);


	void AddMesh(IMesh* mesh, vector<VulkanBuffer> perObjectBuffers) override;

private:

	void CreateBuffers(VulkanMeshData& meshData);
	
	vector<VulkanShader> BaseShadersToVulkanShader(VkDevice device, map<ShaderType, IShader>& shaders);

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

	map<VertexBuffer, vector<VulkanBuffer>> _perObjectBuffer;

	vector<VulkanBuffer> _indices;
	
	map<VertexBuffer, int> _indicesSize;

	vector<VulkanBuffer> _dataBuffers;

	uint32_t _firstBinding;

	size_t _bindingCount;
};
