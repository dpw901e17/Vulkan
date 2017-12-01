#pragma once

#if false
	#define GLFW_INCLUDE_VULKAN	//<-- makes sure glfw includes vulkan
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <GLFW/glfw3.h>	
#else
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <vulkan\vulkan.h>	//<-- can be used to for off-screen rendering
#endif

#include <vector>
#include <memory>
#include "../scene-window-system/Window.h"
#include "../scene-window-system/Scene.h"
#include <glm/glm.hpp>

#include "Buffer.h"
#include <iostream>
#include <iomanip>

class Scene;
struct QueueFamilyIndices;
struct SwapChainSupportDetails;
struct Vertex;


inline std::ostream& operator<<(std::ostream& lhs, const glm::vec3& rhs)
{
	lhs << std::fixed 
		<< "(" << rhs.x << ", " << rhs.y << ", " << rhs.z << ")";
	return lhs;
}

class HelloTriangleApplication {
public:
	explicit HelloTriangleApplication(Scene scene);

	// Does everything!
	void run() {
		initWindow();
		initVulkan();

		updateUniformBuffer();
		updateDynamicUniformBuffer();

		glm::mat4 model_view = m_UniformBufferObject.view * m_InstanceUniformBufferObject.model[0];
		glm::vec3 model_space = { 0.0f, 0.0f, 0.0f };
		glm::vec3 world_space = model_view * glm::vec4(model_space, 1.0f);
		glm::vec3 camera_space = m_UniformBufferObject.projection * model_view * glm::vec4(model_space, 1.0f);

		std::cout << "Model space:  " << model_space << std::endl;
		std::cout << "World space:  " << world_space << std::endl;
		std::cout << "Camera space: " << camera_space << std::endl;

		mainLoop();
		cleanup();
	}

private:
	struct
	{
		glm::mat4 projection;
		glm::mat4 view;
	} m_UniformBufferObject;

	struct
	{
		glm::mat4* model = nullptr;
	} m_InstanceUniformBufferObject;

	Window m_Window;
	VkInstance m_Instance;
	VkPhysicalDevice m_PhysicalDevice = VK_NULL_HANDLE;
	VkDevice m_LogicalDevice; // Logical device. Called device in tut.
	VkQueue m_GraphicsQueue;
	VkSurfaceKHR m_Surface;
	VkQueue m_PresentQueue;

	VkSwapchainKHR m_SwapChain = VK_NULL_HANDLE;
	std::vector<VkImage> m_SwapChainImages;
	VkFormat m_SwapChainImageFormat;
	VkExtent2D m_SwapChainExtent;
	std::vector<VkImageView> m_SwapChainImageViews;
	std::vector<VkFramebuffer> m_SwapChainFramebuffers;

	VkRenderPass m_RenderPass;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	VkPipelineLayout m_PipelineLayout;
	VkPipeline m_GraphicsPipeline;

	VkCommandPool m_CommandPool;
	std::vector<VkCommandBuffer> m_CommandBuffers;

	VkSemaphore m_ImageAvaliableSemaphore;
	VkSemaphore m_RenderFinishedSemaphore;
	VkDebugReportCallbackEXT m_Callback;

	std::unique_ptr<Buffer> m_VertexBuffer;
	std::unique_ptr<Buffer> m_IndexBuffer;
	std::unique_ptr<Buffer> m_UniformBuffer;
	std::unique_ptr<Buffer> m_DynamicUniformBuffer;

	VkDescriptorPool m_DescriptorPool;
	VkDescriptorSet m_DescriptorSet;
	VkImage m_TextureImage;
	VkDeviceMemory m_TextureImageMemory;
	VkImageView m_TextureImageView;
	VkSampler m_TextureSampler;
	VkImage m_DepthImage;
	VkDeviceMemory m_DepthImageMemory;
	VkImageView m_DepthImageView;
	Scene m_Scene;
	uint32_t m_DynamicAllignment;

	static const std::vector<const char*> m_DeviceExtensions;
	static const std::vector<Vertex> m_Vertices;
	static const std::vector<uint16_t> m_Indices;

	void initWindow();

	void createVertexBuffer();
	void createIndexBuffer();
	void createDescriptorSetLayout();
	void createUniformBuffer();
	void setupDebugCallback();
	void createDescriptorPool();
	void createDescriptorSet();
	VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags = VK_IMAGE_ASPECT_COLOR_BIT) const;
	void createTextureImageView();
	void createTextureSampler();
	VkFormat findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const;
	VkFormat findDepthFormat() const;
	void createDepthResources();
	// Initializes Vulkan
	void initVulkan();

	void createSemaphores();

	void createCommandBuffers();

	void createCommandPool();

	void createFramebuffers();

	void createRenderPass();

	void createGraphicsPipeline();

	VkShaderModule createShaderModule(const std::vector<char>& code) const;

	void createImageViews();

	//  Creates and sets the swapchain + sets "swapChainImageFormat" and "swapChainExtent".
	void createSwapChain();

	/**
	* \brief Creates a surface for the window (GLFW handles specifics)
	*/
	void createSurface();

	// Creates and sets "logicalDevice". Also sets "presentQueue" and "graphicsQueue"
	void createLogicalDevice();

	// Finds a suitable physical device with Vulkan support, and sets it to "physicalDevice"
	void pickPhysicalDevice();

	// Finds and returns Queue-families to fill the struct QueueFamilyIndices.
	QueueFamilyIndices findQueueFamilies(const VkPhysicalDevice& device) const;

	// Determines if the given physical device supports both Queue-families and "deviceExtensions"
	bool isDeviceSuitable(const VkPhysicalDevice& device) const;

	// Determines if the physical device supports all extensions in "deviceExtensions"
	static bool checkDeviceExtensionSupport(const VkPhysicalDevice& device);

	// Initializes Vulkan "instance" (think OpenGL context) + loads glfw extensions
	void createInstance();

	// Queries for the capabilities of the physical device, surface format, and present mode
	SwapChainSupportDetails querySwapChainSupport(const VkPhysicalDevice& device) const;

	// Finds and returns the optimal format (colour space + colour format).
	static VkSurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats);

	// Finds and returns the optimal present mode (i.e. how we write to swapchain).
	static VkPresentModeKHR chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes);

	// Finds and returns the "optimal" extent (i.e. resolution) for images in swapchain 
	VkExtent2D chooseSwapExtend(const VkSurfaceCapabilitiesKHR& capabilities) const;
	void updateUniformBuffer();
	void updateDynamicUniformBuffer() const;

	// Handles (window) events
	void mainLoop();

	void drawFrame();

	// Destroys allocated stuff gracefully
	void cleanup();

	void recreateSwapChain();

	void cleanupSwapChain();

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
	void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize);

	void createTextureImage();

	void createImage(uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
