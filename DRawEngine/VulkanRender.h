#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include <glm/mat4x4.hpp> // glm::mat4
#include <glm/vec3.hpp> // glm::vec3
#include <glm/vec4.hpp> // glm::vec4

#include <glm/ext/matrix_transform.hpp> // glm::translate, glm::rotate, glm::scale
#include <glm/ext/matrix_clip_space.hpp> // glm::perspective
#include <glm/gtc/matrix_transform.hpp>

#include "IRender.h"
#include "SwapchainBuffer.h"
#include "IImage.h"
#include "ISwapchain.h"
#include "IPipeline.h"
#include "IFramebuffer.h"
#include "ICommandPool.h"

using namespace std;
using namespace glm;

class VulkanRender : public IRender
{
public:
	VulkanRender();	

	void Init(vector<const char*>* extensions) override;

	~VulkanRender();	

	void InitSurface(int screenWidth, int screenHeight);

	VkInstance GetInstance() const;

	VkSurfaceKHR* GetSurface();

	void CreateDepthBuffer();

	void CreateBuffer();

protected:
	static std::vector<const char*> GetLayers();

	static VkApplicationInfo CreateAppInfo();

	void CreateInstanceCreateInfo(VkApplicationInfo appInfo, vector<const char*>* extensions, vector<const char*>* layers);

	void CreateCommandPool(int queueFamilyIndex);

	static bool IsDeviceSuitable(VkPhysicalDevice device);

	void CreateCommandBuffer();

	void EnumeratePhysicalDevices();	

	void InitDevice();

	VkBool32* GetQueuesSupportPresenting(uint32_t queueFamilyCount) const;
	
	bool GetGraphicsAndPresentQueue(uint32_t queueFamilyCount, vector<VkQueueFamilyProperties> queueProps);

	VkDeviceQueueCreateInfo _queueInfo;

	VkInstance _instance = nullptr;

	VkDevice _device = nullptr;

	VkSurfaceKHR _surface = nullptr;


	ICommandPool* _commandPool;

	VkQueue _drawQueue;	

	VkExtent2D _swapchainExtent;

	VkDebugUtilsMessengerEXT debugMessenger;

	vector<VkPhysicalDevice>* _gpus;

	vector<SwapchainBuffer>* _swapchainBuffers;

	uint32_t _graphicsQueueFamilyIndex;
	
	uint32_t _presentQueueFamilyIndex;

	IImage* _depthBuffer;

	ISwapchain* _swapchain;

	IPipeline* _pipeline;

	IFramebuffer* _framebuffer;

private:
	static const uint32_t INCORRECT_WIDTH = 0xFFFFFFFF;

	vector<const char*> _extensions;
};

