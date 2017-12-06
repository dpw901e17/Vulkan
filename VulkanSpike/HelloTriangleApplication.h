#pragma once

#define VK_USE_PLATFORM_WIN32_KHR
#include <vulkan\vulkan.hpp>

#include <vector>
#include <memory>
#include "../scene-window-system/Window.h"
#include "../scene-window-system/Scene.h"
#include "../scene-window-system/TestConfiguration.h"
#include <glm/glm.hpp>

#include "Buffer.h"
#include "Image.h"
#include "Instance.h"

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
	vk::DescriptorSetLayout m_DescriptorSetLayout;
	vk::PipelineLayout m_PipelineLayout;
	vk::Pipeline m_GraphicsPipeline;

	vk::CommandPool m_CommandPool;
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
	std::unique_ptr<Image> m_DepthImage;
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
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const;
	vk::Format findDepthFormat() const;
	void createDepthResources();
	void createQueryPool();
	void initVulkan();

	void createSemaphores();

	void createCommandBuffers();

	void createCommandPool();

	void createFramebuffers();

	void createRenderPass();

	void createGraphicsPipeline();

	void createImageViews();

	void createSwapChain();

	void createSurface();

	void createLogicalDevice();

	void pickPhysicalDevice();

	QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device) const;

	bool isDeviceSuitable(const vk::PhysicalDevice& device) const;

	static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);

	SwapChainSupportDetails querySwapChainSupport(const vk::PhysicalDevice& device) const;

	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);

	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);

	vk::Extent2D chooseSwapExtend(const vk::SurfaceCapabilitiesKHR& capabilities) const;
	void updateUniformBuffer();
	void updateDynamicUniformBuffer() const;

	void mainLoop();

	void drawFrame();

	void cleanup();

	void recreateSwapChain();

	void cleanupSwapChain();

	void copyBuffer(vk::Buffer source, vk::Buffer destination, vk::DeviceSize);

	void createTextureImage();

	vk::CommandBuffer beginSingleTimeCommands() const;
	void endSingleTimeCommands(vk::CommandBuffer commandBuffer);
	void transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout);
	void copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height);
};
