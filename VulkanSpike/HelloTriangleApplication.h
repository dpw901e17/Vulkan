#pragma once

#if true
	#define GLFW_INCLUDE_VULKAN	//<-- makes sure glfw includes vulkan
	#include <GLFW/glfw3.h>	
#else
	#include <vulkan\vulkan.h>	//<-- can be used to for off-screen rendering
#endif

#include <vector>

struct QueueFamilyIndices;
struct SwapChainSupportDetails;
struct Vertex;


class HelloTriangleApplication {
public:
	// Does everything!
	void run() {
		initWindow();
		initVulkan();
		mainLoop();
		cleanup();
	}

private:
	GLFWwindow* window;
	VkInstance instance;
	VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
	VkDevice logicalDevice; // Logical device. Called device in tut.
	VkQueue graphicsQueue;
	VkSurfaceKHR surface;
	VkQueue presentQueue;
	VkSwapchainKHR swapChain;
	std::vector<VkImage> swapChainImages;
	VkFormat swapChainImageFormat;
	VkExtent2D swapChainExtent;
	std::vector<VkImageView> swapChainImageViews;
	VkRenderPass renderPass;
	VkDescriptorSetLayout descriptorSetLayout;
	VkPipelineLayout pipelineLayout;
	VkPipeline graphicsPipeline;
	std::vector<VkFramebuffer> swapChainFramebuffers;
	VkCommandPool commandPool;
	std::vector<VkCommandBuffer> commandBuffers;
	VkSemaphore imageAvaliableSemaphore;
	VkSemaphore renderFinishedSemaphore;
	VkDebugReportCallbackEXT callback;

	VkBuffer vertexBuffer;
	VkDeviceMemory vertexBufferMemory;
	VkBuffer indexBuffer;
	VkDeviceMemory indexBufferMemory;
	VkBuffer uniformBuffer;
	VkDeviceMemory uniformBufferMemory;

	static const std::vector<const char*> deviceExtensions;
	static const std::vector<Vertex> vertices;
	static const std::vector<uint16_t> indices;

	// Crates a GLFW window (without OpenGL context)
	void initWindow();

	void createVertexBuffer();
	void createIndexBuffer();
	void createDescriptorSetLayout();
	void createUniformBuffer();
	void setupDebugCallback();
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
	// Handles (window) events
	void mainLoop();

	void drawFrame();

	// Destroys allocated stuff gracefully
	void cleanup();

	void recreateSwapChain();

	void cleanupSwapChan();

	static void onWindowResize(GLFWwindow* window, int width, int height);

	uint32_t findMemoryType(uint32_t typeFilter, VkMemoryPropertyFlags properties) const;

	void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory) const;
	void copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize) const;
};