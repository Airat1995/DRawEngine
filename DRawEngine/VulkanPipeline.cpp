#include "VulkanPipeline.h"


VulkanPipeline::VulkanPipeline(VkDevice device, VkPhysicalDevice physical, VulkanRenderpass& renderpass,
	VulkanMeshData& vulkanMeshData, VkExtent2D extent) : _device(device), _physical(physical), _renderPass(renderpass), _meshData(vulkanMeshData)
{
	Initialize(device, vulkanMeshData, extent);	
	CreateBuffers(vulkanMeshData);
	vulkanMeshData.SetBufferRecreateEventListener(this);
}

void VulkanPipeline::Initialize(VkDevice device, VulkanMeshData& vulkanMeshData, VkExtent2D extent)
{
	vector<VkVertexInputAttributeDescription> attributeDescription = vulkanMeshData.AttributeDescriptions();
	auto attributeDescriptions = new VkVertexInputAttributeDescription[attributeDescription.size()];
	for (int attributeIndex = 0; attributeIndex < attributeDescription.size(); ++attributeIndex)
	{
		attributeDescriptions[attributeIndex] = attributeDescription[attributeIndex];
	}

	vector<VkVertexInputBindingDescription> bindingDescription = vulkanMeshData.BindingDescriptions();
	auto* bindingDesctiptions = new VkVertexInputBindingDescription[bindingDescription.size()];
	for (int bindingIndex = 0; bindingIndex < bindingDescription.size(); ++bindingIndex)
	{
		bindingDesctiptions[bindingIndex] = bindingDescription[bindingIndex];
	}

	vector<VulkanBuffer> buffersDescriptions = vulkanMeshData.Buffers();
	vector<VulkanBuffer> perObjectDescriptions = vulkanMeshData.PerObjectBuffersInfo();
	vector<VulkanImage> imagesDescriptions = vulkanMeshData.Images();
	vector<VkDescriptorSetLayoutBinding> descriptorSets = vector<VkDescriptorSetLayoutBinding>();
	for (auto& buffersDescription : buffersDescriptions)
	{
		descriptorSets.push_back(buffersDescription.DescriptorBindingInfo());
	}

	for (vector<VulkanBuffer>::value_type perObject : perObjectDescriptions)
	{
		descriptorSets.push_back(perObject.DescriptorBindingInfo());
	}
	

	for (auto& imageDescription : imagesDescriptions)
	{
		descriptorSets.push_back(imageDescription.DescriptorBindingInfo());
	}

	VkDescriptorSetLayoutCreateInfo layoutInfo{};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = static_cast<uint32_t>(descriptorSets.size());
	layoutInfo.pBindings = descriptorSets.data();
	layoutInfo.flags = 0;
	layoutInfo.pNext = nullptr;

	VkResult result = vkCreateDescriptorSetLayout(_device, &layoutInfo, nullptr, &_descriptorSetLayout);
	if (result != VK_SUCCESS)
		throw runtime_error("Unable to create descriptor set layout");


	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = vulkanMeshData.BindingDescriptions().size();
	vertexInputInfo.vertexAttributeDescriptionCount = vulkanMeshData.AttributeDescriptions().size();
	vertexInputInfo.pVertexAttributeDescriptions = attributeDescriptions;
	vertexInputInfo.pVertexBindingDescriptions = bindingDesctiptions;
	vertexInputInfo.flags = 0;
	vertexInputInfo.pNext = nullptr;

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	VkViewport viewport = {};
	viewport.x = 0.0f;
	viewport.y = static_cast<float>(extent.height);
	viewport.width = static_cast<float>(extent.width);
	viewport.height = -static_cast<float>(extent.height);
	viewport.minDepth = 0.0f;
	viewport.maxDepth = 1.0f;

	VkRect2D scissor = {};
	scissor.offset = { 0, 0 };
	scissor.extent = extent;

	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA;
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA;
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
	colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

	VkPipelineColorBlendStateCreateInfo colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY;
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttachment;
	colorBlending.blendConstants[0] = 0.0f;
	colorBlending.blendConstants[1] = 0.0f;
	colorBlending.blendConstants[2] = 0.0f;
	colorBlending.blendConstants[3] = 0.0f;

	VkPushConstantRange pushConstantRange;
	if (!perObjectDescriptions.empty())
	{
		pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
		pushConstantRange.offset = 0;
		pushConstantRange.size = perObjectDescriptions.at(0).Size();
	}

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &_descriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = !perObjectDescriptions.empty() ? 1 : 0;
	pipelineLayoutInfo.pPushConstantRanges = !perObjectDescriptions.empty() ? &pushConstantRange : nullptr;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	auto shadersInfo = vector<VkPipelineShaderStageCreateInfo>();
	auto basicShaders = _meshData.Meshes()[0]->Shaders();
	auto shaders = BaseShadersToVulkanShader(_device, basicShaders);
	for (auto& shader : shaders)
		shadersInfo.push_back(shader.GetShaderStageInfo());


	VkPipelineDepthStencilStateCreateInfo depthStencil{};
	depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depthStencil.depthTestEnable = VK_TRUE;
	depthStencil.depthWriteEnable = VK_TRUE;
	depthStencil.depthCompareOp = VK_COMPARE_OP_LESS_OR_EQUAL;
	depthStencil.depthBoundsTestEnable = VK_FALSE;
	depthStencil.minDepthBounds = 0.0f;
	depthStencil.maxDepthBounds = 1.0f;
	depthStencil.stencilTestEnable = VK_FALSE;
	depthStencil.front = {};
	depthStencil.back = {};
	
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = shadersInfo.size();
	pipelineInfo.pStages = shadersInfo.data();
	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.layout = _pipelineLayout;
	pipelineInfo.renderPass = _renderPass.RenderPass();
	pipelineInfo.pTessellationState = nullptr;
	pipelineInfo.pDynamicState = nullptr;
	pipelineInfo.pDepthStencilState = &depthStencil;
	pipelineInfo.basePipelineHandle = nullptr;
	pipelineInfo.subpass = 0;
	pipelineInfo.flags = 0;
	pipelineInfo.pNext = nullptr;

	_firstBinding = _meshData.BindingDescriptions()[0].binding;
	_bindingCount = _meshData.BindingDescriptions().size();


	if (vkCreateGraphicsPipelines(device, nullptr, 1, &pipelineInfo, nullptr, &_pipeline) != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}

	for (auto& shader : shaders)
		shader.DestroyShader();


	vector<VkDescriptorPoolSize> typeCounts = vector<VkDescriptorPoolSize>();
	for (auto bufferDescription : buffersDescriptions)
	{
		VkDescriptorPoolSize poolInfo = {};		
		poolInfo.type = bufferDescription.DescriptorBindingInfo().descriptorType;
		poolInfo.descriptorCount = 1;
		typeCounts.push_back(poolInfo);
	}

	for (auto imageDescription : imagesDescriptions)
	{
		VkDescriptorPoolSize poolInfo = {};
		poolInfo.type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		poolInfo.descriptorCount = 1;
		typeCounts.push_back(poolInfo);
	}

	// Create the global descriptor pool
	// All descriptors used in this example are allocated from this pool
	VkDescriptorPoolCreateInfo descriptorPoolInfo = {};
	descriptorPoolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	descriptorPoolInfo.pNext = nullptr;
	descriptorPoolInfo.poolSizeCount = typeCounts.size();
	descriptorPoolInfo.pPoolSizes = typeCounts.data();
	// Set the max. number of descriptor sets that can be requested from this pool (requesting beyond this limit will result in an error)
	descriptorPoolInfo.maxSets = typeCounts.size();

	vkCreateDescriptorPool(device, &descriptorPoolInfo, nullptr, &_descriptorPool);

	// Allocate a new descriptor set from the global descriptor pool
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = _descriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &_descriptorSetLayout;

	result = vkAllocateDescriptorSets(device, &allocInfo, &_descriptorSets);
	if (result != VK_SUCCESS)
		throw std::runtime_error("failed to allocate descriptor sets!");

	// Update the descriptor set determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point
	// 
	vector<VkWriteDescriptorSet> writeDescriptorSet = vector<VkWriteDescriptorSet>();
	for (auto&& bufferDescription : buffersDescriptions)
	{
		VkDescriptorBufferInfo* descr = bufferDescription.BufferDescriptorInfo();
		VkWriteDescriptorSet descriptorSet = {};
		descriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorSet.dstSet = _descriptorSets;
		descriptorSet.descriptorCount = 1;
		descriptorSet.dstArrayElement = 0;
		descriptorSet.descriptorType = bufferDescription.DescriptorBindingInfo().descriptorType;
		descriptorSet.pBufferInfo = descr;
		descriptorSet.dstBinding = bufferDescription.Binding();
		writeDescriptorSet.push_back(descriptorSet);
	}

	for (auto&& imageDescription : imagesDescriptions)
	{
		VkWriteDescriptorSet iamgedescriptorSet = {};
		iamgedescriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		iamgedescriptorSet.dstSet = _descriptorSets;
		iamgedescriptorSet.descriptorCount = 1;
		iamgedescriptorSet.dstArrayElement = 0;
		iamgedescriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
		iamgedescriptorSet.pImageInfo = &imageDescription.ImageInfo();
		iamgedescriptorSet.dstBinding = imageDescription.Binding();
		writeDescriptorSet.push_back(iamgedescriptorSet);
	}

	vkUpdateDescriptorSets(device, writeDescriptorSet.size(), writeDescriptorSet.data(), 0, nullptr);
}

vector<VulkanShader> VulkanPipeline::BaseShadersToVulkanShader(VkDevice device, map<ShaderType, IShader>& shaders)
{
	auto vulkanShaders = vector<VulkanShader>();
	for (auto& shader : shaders)
	{
		vulkanShaders.push_back(VulkanShader(device, shader.second));
	}

	return vulkanShaders;
}

VkPipeline VulkanPipeline::Pipeline()
{
	return _pipeline;
}

VulkanPipeline::~VulkanPipeline()
{
	
}

void VulkanPipeline::BindBuffer(VkCommandBuffer commandBuffer)
{
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets, 0, nullptr);
	for (auto& dataBuffer : _dataBuffers)
	{
		dataBuffer.Fill();
	}
}

void VulkanPipeline::BuildCommandbuffer(VkCommandBuffer commandBuffer)
{	
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	VkDeviceSize offsets[1] = { 0 };
	for (auto& meshBuffer : _meshBuffers)
	{
		bool indexedDraw = _indicesSize.find(meshBuffer) != _indicesSize.end();
		auto constantBuffers = _perObjectBuffer.at(meshBuffer);
		for (auto&& constantBuffer : constantBuffers)
		{
			vkCmdPushConstants(commandBuffer, _pipelineLayout, VK_SHADER_STAGE_VERTEX_BIT, 0, constantBuffer.Size(), constantBuffer.DataLocation());
		}
		vkCmdBindVertexBuffers(commandBuffer, _firstBinding, _bindingCount, &meshBuffer.Buffer(), offsets);
		if(indexedDraw)
		{			
			vkCmdBindIndexBuffer(commandBuffer, _indices[0].Buffer(), 0, VK_INDEX_TYPE_UINT16);
			vkCmdDrawIndexed(commandBuffer, _indicesSize[meshBuffer], 1, 0, 0, 0);
		}
		else
		{
			vkCmdDraw(commandBuffer, meshBuffer.VertexCount(), 1, 0, 0);
		}
	}
}

void VulkanPipeline::DestroyPipeline()
{
	vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
}

void VulkanPipeline::AddMesh(IMesh* mesh, vector<VulkanBuffer> perObjectBuffers)
{
	VertexBuffer* vertexBuffer = new VertexBuffer(_device, _physical, mesh->RequiredBufferSize(), mesh->VertexCount());
	vertexBuffer->Fill(mesh->VerticesData());
	_meshBuffers.push_back(*vertexBuffer);
	_perObjectBuffer.insert({ *vertexBuffer, perObjectBuffers });

}

void VulkanPipeline::CreateBuffers(VulkanMeshData& meshData)
{
	for (IMesh* mesh : meshData.Meshes())
	{
		VertexBuffer* vertexBuffer = new VertexBuffer(_device, _physical, mesh->RequiredBufferSize(), mesh->VertexCount());
		vertexBuffer->Fill(mesh->VerticesData());
		_meshBuffers.push_back(*vertexBuffer);
		_perObjectBuffer.insert({ *vertexBuffer, meshData.PerObjectBuffersInfo(mesh) });

		if (mesh->IndexSize() > 0)
		{
			VulkanBuffer indexedBuffer = VulkanBuffer(_device, _physical, BufferStageFlag::Vertex,
				BufferUsageFlag::IndexBuffer, BufferSharingMode::Exclusive,
				mesh->IndicesData(), mesh->IndexSize() * sizeof(uint16_t), 0);

			indexedBuffer.Fill();
			
			_indices.push_back(indexedBuffer);
			_indicesSize.insert({ *vertexBuffer, mesh->IndexSize() });
		}
	}

	for (const VulkanBuffer& data : meshData.Buffers())
	{
		_dataBuffers.push_back(data);
	}
}
