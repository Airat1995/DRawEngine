#include "VulkanOmniShadowmap.h"

#include "ISwapchain.h"

VulkanOmniShadowmap::VulkanOmniShadowmap(vec4* lightPosition, CameraObject* currentCamera, vector<VulkanMeshData*>* meshDataCollection, VulkanCommandPool* commandPool,
                                         ISwapchain* swapchain, VkDevice device, VkPhysicalDevice gpu, int drawQueue, int presentQueue, int binding, int graphicsFamilyIndex,
                                         int width, int height) : IShadowmap(width, height, binding), _device(device), _gpu(gpu), _lightPosition(lightPosition), _currentCamera(currentCamera), _swapchain(swapchain), _drawQueue(drawQueue), _presentQueue(presentQueue)
{
	_depthBuffer = new VulkanDepthBuffer(device, gpu, width, height, true);
	_image = new BufferlessVulkanImage(commandPool, ImageFormat::R,
		ImageType::Cube, ImageUsage::TransferDst | ImageUsage::Sampler | ImageUsage::ColorAttachment,
		BufferStageFlag::Fragment, width, width, 6, device, gpu, binding, graphicsFamilyIndex, 1);

	_offscreenRenderpass = new VulkanRenderpass(device, VK_FORMAT_R16_SFLOAT, _depthBuffer->Format());

	_frameBuffer = new VulkanFramebuffer(_device, drawQueue, presentQueue, *swapchain, *_image, *_offscreenRenderpass, *commandPool, *_depthBuffer);


	vector<BaseVertex>* vertices = new vector<BaseVertex>();
	for (size_t meshIndex = 0; meshIndex < meshDataCollection->size(); meshIndex++)
	{
		VulkanMeshData* meshData = meshDataCollection->at(meshIndex);
		for (auto& mesh : meshData->Meshes()) 
		{
			vector<vec4>* vertexDatas = mesh->VertexPositions();
			for (auto& verticie : *vertexDatas)
			{
				VertexData* vertexData = new VertexData();
				vertexData->position = verticie;

				BaseVertex* vertex = new BaseVertex(*vertexData);
				vertices->push_back(*vertex);
			}
		}
	}

	IShader vertexShader = IShader(_vertexLoc, _shaderName, ShaderType::Vertex);
	IShader fragmentShader = IShader(_fragmentLoc, _shaderName, ShaderType::Fragment);
	map<ShaderType, IShader> shaders = map<ShaderType, IShader>();
	shaders.insert(shaders.end(), std::pair<ShaderType, IShader>(ShaderType::Vertex, vertexShader));
	shaders.insert(shaders.end(), std::pair<ShaderType, IShader>(ShaderType::Fragment, fragmentShader));

	IMaterial* shadowMapMaterial = new IMaterial(shaders, PreOpaque);
	Mesh<BaseVertex, VertexData>* skyboxMesh = new Mesh<BaseVertex, VertexData>(*vertices, shadowMapMaterial);
	_shadowmapBuffer = ShadowmapBuffer();
	IBuffer* shadowmapBuffer = new BaseBuffer<ShadowmapBuffer>(BufferUsageFlag::UniformBuffer,
	                                                           BufferSharingMode::Exclusive, &_shadowmapBuffer,
	                                                           BufferStageFlag::Vertex, 0);
	
	skyboxMesh->Material()->AddBuffer(shadowmapBuffer);
	
	CreateVulkanPipeline(skyboxMesh);
}


void VulkanOmniShadowmap::Draw(ICommandBuffer* commandBuffer, int index)
{
	VulkanCommandBuffer* vulkanCommandBuffer = static_cast<VulkanCommandBuffer*>(commandBuffer);
	_offscreenRenderpass->BeginRenderPass(_width, _height, *_frameBuffer->Framebuffer(index), vulkanCommandBuffer->CommandBuffer());
	_pipeline->BindPipeline(vulkanCommandBuffer->CommandBuffer());
	_pipeline->BindBuffer(vulkanCommandBuffer->CommandBuffer());

	_pipeline->BuildCommandbuffer(vulkanCommandBuffer->CommandBuffer());
	_offscreenRenderpass->EndRenderPass(vulkanCommandBuffer->CommandBuffer());

	VkImageSubresourceRange cubeFaceSubresourceRange = {};
	cubeFaceSubresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	cubeFaceSubresourceRange.baseMipLevel = 0;
	cubeFaceSubresourceRange.levelCount = 1;
	cubeFaceSubresourceRange.baseArrayLayer = _indexFace;
	cubeFaceSubresourceRange.layerCount = 1;

	SetImageLayout(
		vulkanCommandBuffer->CommandBuffer(),
		_image->Image(),
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		cubeFaceSubresourceRange,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);

	VkImageCopy copyRegion = {};

	copyRegion.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.srcSubresource.baseArrayLayer = 0;
	copyRegion.srcSubresource.mipLevel = 0;
	copyRegion.srcSubresource.layerCount = 1;
	copyRegion.srcOffset = { 0, 0, 0 };

	copyRegion.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	copyRegion.dstSubresource.baseArrayLayer = _indexFace;
	copyRegion.dstSubresource.mipLevel = 0;
	copyRegion.dstSubresource.layerCount = 1;
	copyRegion.dstOffset = { 0, 0, 0 };

	copyRegion.extent.width = _width;
	copyRegion.extent.height = _height;
	copyRegion.extent.depth = 1;

	vkCmdCopyImage(
		vulkanCommandBuffer->CommandBuffer(),
		_image->Image(),
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		*_swapchain->SwapchainBuffers().at(0).Image(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		1,
		&copyRegion);

	SetImageLayout(
		vulkanCommandBuffer->CommandBuffer(),
		*_swapchain->SwapchainBuffers().at(0).Image(),
		VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
		VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL,
		cubeFaceSubresourceRange,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);


	SetImageLayout(
		vulkanCommandBuffer->CommandBuffer(),
		_image->Image(),
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
		VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL,
		cubeFaceSubresourceRange,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT,
		VK_PIPELINE_STAGE_ALL_COMMANDS_BIT);
}

void VulkanOmniShadowmap::Update(int indexFace)
{
	_indexFace = indexFace;
	glm::mat4 viewMatrix = glm::mat4(1.0f);
	switch (indexFace)
	{
	case 0: // POSITIVE_X
		viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 1:	// NEGATIVE_X
		viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
		viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 2:	// POSITIVE_Y
		viewMatrix = glm::rotate(viewMatrix, glm::radians(-90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 3:	// NEGATIVE_Y
		viewMatrix = glm::rotate(viewMatrix, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 4:	// POSITIVE_Z
		viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
		break;
	case 5:	// NEGATIVE_Z
		viewMatrix = glm::rotate(viewMatrix, glm::radians(180.0f), glm::vec3(0.0f, 0.0f, 1.0f));
		break;
	}
	_shadowmapBuffer.ViewMatrix = viewMatrix;
	_shadowmapBuffer.ProjectionMatrix = _currentCamera->_proj;
	_shadowmapBuffer.ModelMatrix = _currentCamera->_model;
	_shadowmapBuffer.LightPosition = *_lightPosition;
}

void VulkanOmniShadowmap::Submit(int index)
{
	_frameBuffer->SubmitFramebuffer(index);
}

void VulkanOmniShadowmap::CreateVulkanPipeline(IMesh* mesh)
{
	vector<VulkanBuffer> buffers{};
	for (vector<IBuffer*>::value_type& buffer : mesh->PerObjectBuffers())
	{
		VulkanBuffer meshBuffer = VulkanBuffer(_device, _gpu, buffer->StageFlag(),
			buffer->Usage(), buffer->SharingMode(),
			buffer->RawData(), buffer->Size(), buffer->BindingId());
		buffers.push_back(meshBuffer);
	}


	vector<VulkanBuffer> vulkanBuffers = vector<VulkanBuffer>();
	for (auto* buffer : mesh->Material()->Buffers())
	{
		VulkanBuffer bufferData = VulkanBuffer(_device, _gpu, buffer->StageFlag(), buffer->Usage(), buffer->SharingMode(), buffer->RawData(), buffer->Size(), buffer->BindingId());;
		vulkanBuffers.push_back(bufferData);
	}

	vector<VulkanImage> images = vector<VulkanImage>();

	VkExtent2D extent;
	extent.width = _width;
	extent.height = _height;

	VulkanMeshData* currentMeshData = new VulkanMeshData(mesh, vulkanBuffers, images, buffers);
	_pipeline = new VulkanPipeline(_device, _gpu, *_offscreenRenderpass, *currentMeshData, extent);
}


void VulkanOmniShadowmap::SetImageLayout(
	VkCommandBuffer cmdbuffer,
	VkImage image,
	VkImageLayout oldImageLayout,
	VkImageLayout newImageLayout,
	VkImageSubresourceRange subresourceRange,
	VkPipelineStageFlags srcStageMask,
	VkPipelineStageFlags dstStageMask)
{
	// Create an image barrier object
	VkImageMemoryBarrier imageMemoryBarrier{};
	imageMemoryBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	imageMemoryBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	imageMemoryBarrier.oldLayout = oldImageLayout;
	imageMemoryBarrier.newLayout = newImageLayout;
	imageMemoryBarrier.image = image;
	imageMemoryBarrier.subresourceRange = subresourceRange;

	// Source layouts (old)
	// Source access mask controls actions that have to be finished on the old layout
	// before it will be transitioned to the new layout
	switch (oldImageLayout)
	{
	case VK_IMAGE_LAYOUT_UNDEFINED:
		// Image layout is undefined (or does not matter)
		// Only valid as initial layout
		// No flags required, listed only for completeness
		imageMemoryBarrier.srcAccessMask = 0;
		break;

	case VK_IMAGE_LAYOUT_PREINITIALIZED:
		// Image is preinitialized
		// Only valid as initial layout for linear images, preserves memory contents
		// Make sure host writes have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image is a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image is a depth/stencil attachment
		// Make sure any writes to the depth/stencil buffer have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image is a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image is a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image is read by a shader
		// Make sure any shader reads from the image have been finished
		imageMemoryBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Target layouts (new)
	// Destination access mask controls the dependency for the new image layout
	switch (newImageLayout)
	{
	case VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL:
		// Image will be used as a transfer destination
		// Make sure any writes to the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL:
		// Image will be used as a transfer source
		// Make sure any reads from the image have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
		break;

	case VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL:
		// Image will be used as a color attachment
		// Make sure any writes to the color buffer have been finished
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL:
		// Image layout will be used as a depth/stencil attachment
		// Make sure any writes to depth/stencil buffer have been finished
		imageMemoryBarrier.dstAccessMask = imageMemoryBarrier.dstAccessMask | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;
		break;

	case VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL:
		// Image will be read in a shader (sampler, input attachment)
		// Make sure any writes to the image have been finished
		if (imageMemoryBarrier.srcAccessMask == 0)
		{
			imageMemoryBarrier.srcAccessMask = VK_ACCESS_HOST_WRITE_BIT | VK_ACCESS_TRANSFER_WRITE_BIT;
		}
		imageMemoryBarrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;
		break;
	default:
		// Other source layouts aren't handled (yet)
		break;
	}

	// Put barrier inside setup command buffer
	vkCmdPipelineBarrier(
		cmdbuffer,
		srcStageMask,
		dstStageMask,
		0,
		0, nullptr,
		0, nullptr,
		1, &imageMemoryBarrier);
}

