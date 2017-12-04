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
#include "../scene-window-system/TestConfiguration.h"
#include <glm/glm.hpp>

#include "Buffer.h"
#include <iostream>
#include "Image.h"

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
	void run();

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
	vk::Instance m_Instance;
	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_LogicalDevice;
	vk::Queue m_GraphicsQueue;
	vk::SurfaceKHR m_Surface;
	vk::Queue m_PresentQueue;

	vk::SwapchainKHR m_SwapChain;
	std::vector<vk::Image> m_SwapChainImages;
	vk::Format m_SwapChainImageFormat;
	vk::Extent2D m_SwapChainExtent;
	std::vector<vk::ImageView> m_SwapChainImageViews;
	std::vector<vk::Framebuffer> m_SwapChainFramebuffers;

	vk::RenderPass m_RenderPass;
	VkDescriptorSetLayout m_DescriptorSetLayout;
	vk::PipelineLayout m_PipelineLayout;
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
	std::unique_ptr<Image> m_TextureImage;
	VkSampler m_TextureSampler;
	std::unique_ptr<Image> m_DepthImage;
	Scene m_Scene;
	uint32_t m_DynamicAllignment;
	vk::QueryPool m_QueryPool;
	TestConfiguration m_testConfiguration;

	static const std::vector<const char*> s_DeviceExtensions;
	static const std::vector<Vertex> s_Vertices;
	static const std::vector<uint16_t> s_Indices;

	void createVertexBuffer();
	void createIndexBuffer();
	void createDescriptorSetLayout();
	void createUniformBuffer();
	void setupDebugCallback();
	void createDescriptorPool();
	void createDescriptorSet();
	vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor) const;
	void createTextureSampler();
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
	vk::Format findDepthFormat() const;
	void createDepthResources();
	void createQueryPool();
	// Initializes Vulkan
	void initVulkan();

	void createSemaphores();

	void createCommandBuffers();

	void createCommandPool();

	void createFramebuffers();

	void createRenderPass();

	void createGraphicsPipeline();

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
	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;

	// Determines if the given physical device supports both Queue-families and "deviceExtensions"
	bool isDeviceSuitable(const vk::PhysicalDevice& device) const;

	// Determines if the physical device supports all extensions in "deviceExtensions"
	static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);

	// Initializes Vulkan "instance" (think OpenGL context) + loads glfw extensions
	void createInstance();

	// Queries for the capabilities of the physical device, surface format, and present mode
	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;

	// Finds and returns the optimal format (colour space + colour format).
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

	// Finds and returns the optimal present mode (i.e. how we write to swapchain).
	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

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

	void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize);

	void createTextureImage();

	VkCommandBuffer beginSingleTimeCommands();
	void endSingleTimeCommands(VkCommandBuffer commandBuffer);
	void transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);
	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);
};
