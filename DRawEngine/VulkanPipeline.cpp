#include "VulkanPipeline.h"

#include "SimpleVertex.h"


VulkanPipeline::VulkanPipeline(VkDevice device, VkPhysicalDevice physical, VulkanRenderpass& renderpass,
	VulkanMeshData& vulkanMeshData, VkFormat imageFormat, VkExtent2D extent) : _physical(physical), _renderPass(renderpass), _meshData(vulkanMeshData), _device(device)
{
	Initialize(device, vulkanMeshData, imageFormat, extent);
	CreateBuffers(vulkanMeshData);
}

void VulkanPipeline::Initialize(VkDevice device, VulkanMeshData& vulkanMeshData, VkFormat imageFormat, VkExtent2D extent)
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
	auto* descriptorSets = new VkDescriptorSetLayout[buffersDescriptions.size()];
	for (int bufferIndex = 0; bufferIndex < buffersDescriptions.size(); ++bufferIndex)
	{
		descriptorSets[bufferIndex] = buffersDescriptions[bufferIndex].DescriptorSetLayout();
	}

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
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(extent.width);
	viewport.height = static_cast<float>(extent.height);
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
	rasterizer.frontFace = VK_FRONT_FACE_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;

	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;

	VkPipelineColorBlendAttachmentState colorBlendAttachment = {};
	colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttachment.blendEnable = VK_TRUE;
	colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_SRC_ALPHA,
	colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ONE_MINUS_SRC_ALPHA,
	colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD,
	colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE,
	colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO,
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

	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = buffersDescriptions.size();
	pipelineLayoutInfo.pSetLayouts = descriptorSets;
	pipelineLayoutInfo.pushConstantRangeCount = 0;

	if (vkCreatePipelineLayout(device, &pipelineLayoutInfo, nullptr, &_pipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	auto shadersInfo = vector<VkPipelineShaderStageCreateInfo>();
	auto basicShaders = _meshData.Meshes()[0]->Shaders();
	auto shaders = BaseShadersToVulkanShader(_device, basicShaders);
	for (auto& shader : shaders)
		shadersInfo.push_back(shader.GetShaderStageInfo());


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
	pipelineInfo.pDepthStencilState = nullptr;
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

		
	vector<VkDescriptorPoolSize> typeCounts = vector<VkDescriptorPoolSize>(buffersDescriptions.size());
	for (auto bufferDescription : buffersDescriptions)
	{
		VkDescriptorPoolSize poolInfo = {};
		poolInfo.type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
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
	allocInfo.pSetLayouts = descriptorSets;

	VkResult result = vkAllocateDescriptorSets(device, &allocInfo, &_descriptorSets);

	// Update the descriptor set determining the shader binding points
	// For every binding point used in a shader there needs to be one
	// descriptor set matching that binding point
	// 
	vector<VkWriteDescriptorSet> writeDescriptorSet = vector<VkWriteDescriptorSet>();
	for (auto bufferDescription : buffersDescriptions)
	{
		VkWriteDescriptorSet descriptorSet = {};
		// Binding 0 : Uniform buffer
		descriptorSet.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorSet.dstSet = _descriptorSets;
		descriptorSet.descriptorCount = 1;
		descriptorSet.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
		descriptorSet.pBufferInfo = &bufferDescription.BufferDescriptorInfo();
		// Binds this uniform buffer to binding point 0
		descriptorSet.dstBinding = bufferDescription.BindingId();
		writeDescriptorSet.push_back(descriptorSet);
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

VkDescriptorType VulkanPipeline::BufferUsageToDescriptorType(BufferUsageFlag bufferUsageFlag)
{
	VkDescriptorType descriptor;
	switch (bufferUsageFlag)
	{
	case BufferUsageFlag::UniformTexel: descriptor = VK_DESCRIPTOR_TYPE_UNIFORM_TEXEL_BUFFER; break;
	case BufferUsageFlag::StorageTexel: descriptor = VK_DESCRIPTOR_TYPE_STORAGE_TEXEL_BUFFER; break;
	case BufferUsageFlag::UniformBuffer: descriptor = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER; break;
	case BufferUsageFlag::StorageBuffer: descriptor = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER; break;
	default: cerr << "Current type not supported! Please add new case statement in VulkanPipeline->BufferUsageToDescriptorType" << endl;
	}

	return descriptor;
}

VkPipeline VulkanPipeline::Pipeline()
{
	return _pipeline;
}


VulkanPipeline::~VulkanPipeline()
{
	
}

void VulkanPipeline::DrawFrame(VkCommandBuffer commandBuffer)
{
	vkCmdBindDescriptorSets(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipelineLayout, 0, 1, &_descriptorSets, 0, nullptr);
	for (auto& dataBuffer : _dataBuffers)
	{
		dataBuffer.Fill();
	}
	vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, _pipeline);
	VkDeviceSize offsets[1] = { 0 };
	for (auto& _meshBuffer : _meshBuffers)
	{
		vkCmdBindVertexBuffers(commandBuffer, _firstBinding, _bindingCount, &_meshBuffer.Buffer(), offsets);
		vkCmdDraw(commandBuffer, _meshBuffer.VertexCount(), 1, 0, 0);
	}
}

void VulkanPipeline::DestroyPipeline()
{
	vkDestroyPipelineLayout(_device, _pipelineLayout, nullptr);
}

void VulkanPipeline::CreateBuffers(VulkanMeshData& meshData)
{
	for (IMesh* mesh : meshData.Meshes())
	{
		VertexBuffer vertexBuffer = VertexBuffer(_device, _physical, mesh->RequiredBufferSize(), mesh->VertexCount());
		vertexBuffer.Fill(mesh->VerticesData());
		_meshBuffers.push_back(vertexBuffer);
	}

	for (VulkanBuffer data : meshData.Buffers())
	{
		_dataBuffers.push_back(data);
	}
}
