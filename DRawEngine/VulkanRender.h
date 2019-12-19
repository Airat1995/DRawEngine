#pragma once
#include <vector>
#include <vulkan/vulkan.h>

#include "IRender.h"
#include "SwapchainBuffer.h"

using namespace std;

class VulkanRender : public IRender
{
public:
	VulkanRender();	

	void Init(vector<const char*>* extensions) override;

	~VulkanRender();	

	void InitSurface(int screenWidth, int screenHeight);

	VkInstance GetInstance() const;

	VkSurfaceKHR* GetSurface();

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

	VkSwapchainKHR _swapChain = nullptr;

	VkQueue _drawQueue;	

	VkExtent2D _swapchainExtent;

	VkDebugUtilsMessengerEXT debugMessenger;

	vector<VkPhysicalDevice>* _gpus;

	vector<SwapchainBuffer>* _swapchainBuffers;

	uint32_t _graphicsQueueFamilyIndex;
	
	uint32_t _presentQueueFamilyIndex;
private:
	static const uint32_t INCORRECT_WIDTH = 0xFFFFFFFF;

	vector<const char*> _extensions;
};

