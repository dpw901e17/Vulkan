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
	explicit HelloTriangleApplication(Scene);

	// Does everything!
	void run();

private:
	static vk::DeviceSize dynamicBufferSize(const Scene&, const Device&);
	struct
	{
		glm::mat4 projection;
		glm::mat4 view;
	} m_UniformBufferObject;

	struct
	{
		glm::mat4* model = nullptr;
	} m_InstanceUniformBufferObject;

	Scene m_Scene;
	Window m_Window;
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

	Buffer m_VertexBuffer;
	Buffer m_IndexBuffer;
	Buffer m_UniformBuffer;
	Buffer m_DynamicUniformBuffer;

	vk::DescriptorPool m_DescriptorPool;
	vk::DescriptorSet m_DescriptorSet;
	Image m_TextureImage;
	vk::Sampler m_TextureSampler;
	vk::QueryPool m_QueryPool;
	TestConfiguration m_testConfiguration;

	static const std::vector<const char*> s_DeviceExtensions;
	static const std::vector<Vertex> s_Vertices;
	static const std::vector<uint16_t> s_Indices;

	void createDescriptorSetLayout();
	void createDescriptorPool();
	void createDescriptorSet();
	void createTextureSampler();
	void createQueryPool();
	// Initializes Vulkan
	void initVulkan();

	void createSemaphores();

	void createCommandBuffers();

	void createGraphicsPipeline();

	static vk::SurfaceKHR createSurface(const Window&, const Instance&);

	void updateUniformBuffer();
	void updateDynamicUniformBuffer() const;

	// Handles (window) events
	void mainLoop();

	void drawFrame();

	// Destroys allocated stuff gracefully
	void cleanup() const;

	void cleanupSwapChain() const;
};
