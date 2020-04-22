#pragma once
#include <vulkan/vulkan.h>
#include <filesystem>
#include <glm/fwd.hpp>
#include <map>
#include "IRender.h"
#include "VulkanPipeline.h"
#include "VulkanCommandBuffer.h"
#include "IMesh.h"
#include "ISwapchain.h"
#include "VulkanCommandPool.h"
#include "VulkanFramebuffer.h"
#include "IImage.h"

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

	void CreateDepthBuffer();

	void DrawFrame() override;

	void AddMesh(IMesh* mesh) override;

	void RemoveMesh(IMesh* mesh) override;

	VkDevice& Device();

	glm::uint32 GraphicsQueueFamilyIndex();

	uint32_t PresentQueueFamilyIndex();
	VkPhysicalDevice Physical();
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

	VulkanCommandPool* _commandPool;

	VkExtent2D _swapchainExtent;

	VkDebugUtilsMessengerEXT debugMessenger;

	vector<VkPhysicalDevice> _gpus;

	uint32_t _graphicsQueueFamilyIndex;
	
	uint32_t _presentQueueFamilyIndex;

	IImage* _depthBuffer;

	VulkanFramebuffer* _framebuffer;
	
	ISwapchain* _swapchain;
	
	vector<VulkanPipeline> _pipelines{};

	VulkanRenderpass* _renderpass;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::ofstream outfile;
		outfile.open("log.txt", std::ios_base::app); // append instead of overwrite
		outfile << pCallbackData->pMessage << endl;
		outfile.close();
		return VK_FALSE;
	}

	void SetupDebugMessenger();

	VkResult CreateDebugUtilsMessengerEXT(VkInstance instance, const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugUtilsMessengerEXT* pDebugMessenger) {
		auto func = PFN_vkCreateDebugUtilsMessengerEXT(vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT"));
		if (func != nullptr) {
			return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
		}
		else {
			return VK_ERROR_EXTENSION_NOT_PRESENT;
		}
	}

private:
	static const uint32_t INCORRECT_WIDTH = 0xFFFFFFFF;

	vector<const char*> _extensions;

	int _width;

	int _height;
};

