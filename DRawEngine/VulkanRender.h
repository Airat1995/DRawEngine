#pragma once
#include <filesystem>
#include <glm/fwd.hpp>
#include "IRender.h"
#include "VulkanPipeline.h"
#include "IMesh.h"
#include "ISwapchain.h"
#include "VulkanCommandPool.h"
#include "VulkanFramebuffer.h"
#include "VulkanBuffer.h"
#include "VulkanDepthBuffer.h"


using namespace std;

class VulkanRender : public IRender
{
public:
	VulkanRender();	

	void Init() override
	{
	}	
	
	void Init(vector<const char*>* extensions);

	~VulkanRender();	

	void InitSurface(int screenWidth, int screenHeight);

	VkInstance GetInstance() const;

	VkSurfaceKHR* GetSurface();

	void DrawFrame() override;

	void AddMesh(IMesh* mesh) override;

	void RemoveMesh(IMesh* mesh) override;

	VkDevice& Device();

	uint32_t GraphicsQueueFamilyIndex();

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

	VulkanDepthBuffer* _depthBuffer;

	VulkanFramebuffer* _framebuffer;
	
	ISwapchain* _swapchain;
	
	vector<VulkanPipeline*> _pipelines{};

	vector<VulkanMeshData*> _meshDataCollection{};

	VulkanRenderpass* _renderpass;

	vector<IBuffer> _mapBuffers;

	VulkanBuffer* _buffer;

	static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
		VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
		VkDebugUtilsMessageTypeFlagsEXT messageType,
		const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
		void* pUserData) {

		std::ofstream outfile;
		outfile.open(DEBUG_FILENAME.c_str(), std::ios_base::app); // append instead of overwrite
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
	
	inline static const string DEBUG_FILENAME = "debugLog.log";

	vector<const char*> _extensions;

	int _width;

	int _height;

};

