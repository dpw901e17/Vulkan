#pragma once

#if false
	#define GLFW_INCLUDE_VULKAN	//<-- makes sure glfw includes vulkan
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <GLFW/glfw3.h>	
#else
	#define VK_USE_PLATFORM_WIN32_KHR
	#include <vulkan\vulkan.hpp>	//<-- can be used to for off-screen rendering
#endif

#include <vector>
#include <memory>
#include "../scene-window-system/Window.h"
#include "../scene-window-system/Scene.h"
#include "../scene-window-system/TestConfiguration.h"
#include <glm/glm.hpp>

#include "Buffer.h"
#include "CommandPool.h"
#include "Device.h"
#include "Image.h"
#include "Instance.h"
#include "Swapchain.h"

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
	Scene m_Scene;
	Instance m_Instance;
	vk::SurfaceKHR m_Surface;
	Device m_Device;
	CommandPool m_CommandPool;
	Swapchain m_SwapChain;

	vk::DescriptorSetLayout m_DescriptorSetLayout;
	vk::PipelineLayout m_PipelineLayout;
	vk::Pipeline m_GraphicsPipeline;

	std::vector<vk::CommandBuffer> m_CommandBuffers;

	vk::Semaphore m_ImageAvaliableSemaphore;
	vk::Semaphore m_RenderFinishedSemaphore;

	std::unique_ptr<Buffer> m_VertexBuffer;
	std::unique_ptr<Buffer> m_IndexBuffer;
	std::unique_ptr<Buffer> m_UniformBuffer;
	std::unique_ptr<Buffer> m_DynamicUniformBuffer;

	vk::DescriptorPool m_DescriptorPool;
	vk::DescriptorSet m_DescriptorSet;
	std::unique_ptr<Image> m_TextureImage;
	vk::Sampler m_TextureSampler;
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
	void createDescriptorPool();
	void createDescriptorSet();
	vk::ImageView createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect_flags = vk::ImageAspectFlagBits::eColor) const;
	void createTextureSampler();
	void createQueryPool();
	// Initializes Vulkan
	void initVulkan();

	void createSemaphores();

	void createCommandBuffers();

	void createGraphicsPipeline();

	/**
	* \brief Creates a surface for the window (GLFW handles specifics)
	*/
	static vk::SurfaceKHR createSurface(const Window&, const Instance&);

	// Determines if the given physical device supports both Queue-families and "deviceExtensions"
	bool isDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR&, const QueueFamilyIndices& indices) const;

	// Determines if the physical device supports all extensions in "deviceExtensions"
	static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);

	// Queries for the capabilities of the physical device, surface format, and present mode
	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;

	// Finds and returns the optimal format (colour space + colour format).
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

	// Finds and returns the optimal present mode (i.e. how we write to swapchain).
	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

	// Finds and returns the "optimal" extent (i.e. resolution) for images in swapchain 
	vk::Extent2D chooseSwapExtend(const vk::SurfaceCapabilitiesKHR& capabilities) const;
	void updateUniformBuffer();
	void updateDynamicUniformBuffer() const;

	// Handles (window) events
	void mainLoop();

	void drawFrame();

	// Destroys allocated stuff gracefully
	void cleanup();

	void recreateSwapChain();

	void cleanupSwapChain();

	void createTextureImage();
};
