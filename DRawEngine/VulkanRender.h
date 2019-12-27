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

	VkFormat GetSupportedFormat();

	void EnumeratePhysicalDevices();	

	void InitDevice();

	VkBool32* GetQueuesSupportPresenting(uint32_t queueFamilyCount) const;
	
	bool GetGraphicsAndPresentQueue(uint32_t queueFamilyCount, vector<VkQueueFamilyProperties> queueProps);

	static VkSurfaceTransformFlagBitsKHR GetPresentMode(VkSurfaceCapabilitiesKHR surfaceCapabilities);

	VkCompositeAlphaFlagBitsKHR GetCompositeAlpha(VkSurfaceCapabilitiesKHR surfaceCapabilities);

	void CreateSwapchainAndImages(VkSwapchainCreateInfoKHR swapchainInfo);

	VkDeviceQueueCreateInfo _queueInfo;

	VkInstance _instance = nullptr;

	VkDevice _device = nullptr;

	VkSurfaceKHR _surface = nullptr;

	VkCommandBuffer _commandBuffer = nullptr;

	VkCommandPool _commandPool = nullptr;

	VkQueue _drawQueue;	

	VkExtent2D _swapchainExtent;

	VkDebugUtilsMessengerEXT debugMessenger;

	vector<VkPhysicalDevice>* _gpus;

	vector<SwapchainBuffer>* _swapchainBuffers;

	uint32_t _graphicsQueueFamilyIndex;
	
	uint32_t _presentQueueFamilyIndex;

	IImage* _depthBuffer;

	ISwapchain* _swapchain;

private:
	static const uint32_t INCORRECT_WIDTH = 0xFFFFFFFF;

	vector<const char*> _extensions;
};

