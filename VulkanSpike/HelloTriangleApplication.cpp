#include "HelloTriangleApplication.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define STB_IMAGE_IMPLEMENTATION

#include <windows.h>	// For Beeps included early for not redifining max

#include <algorithm>
#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <set>
#include <stb/stb_image.h>
#include <thread>		// For non blocking beeps
#include <vulkan/vulkan.h>

#include "SwapChainSupportDetails.h"
#include "QueueFamilyIndices.h"
#include "Vertex.h"
#include "Shader.h"
#include "Image.h"

const int WIDTH = 800;
const int HEIGHT = 600;

const std::vector<const char*> validationLayers = {
	"VK_LAYER_LUNARG_standard_validation"
};

#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif

#define GET_INSTANCE_PROC(name) (PFN_##name)vkGetInstanceProcAddr(instance, #name);

VkResult CreateDebugReportCallbackEXT(VkInstance instance, const VkDebugReportCallbackCreateInfoEXT* pCreateInfo, const VkAllocationCallbacks* pAllocator, VkDebugReportCallbackEXT* pCallback) {
	static auto func = GET_INSTANCE_PROC(vkCreateDebugReportCallbackEXT);
	if (func != nullptr) {
		return func(instance, pCreateInfo, pAllocator, pCallback);
	}
	else {
		return VK_ERROR_EXTENSION_NOT_PRESENT;
	}
}

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator) {
	static auto func = GET_INSTANCE_PROC(vkDestroyDebugReportCallbackEXT);
	if (func != nullptr) {
		func(instance, callback, pAllocator);
	}
}

bool checkValidationLayerSupport() {
	uint32_t layerCount;
	vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

	std::vector<VkLayerProperties> availableLayers(layerCount);
	vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

	for (auto layerName : validationLayers) {
		auto layerFound = false;

		for (const auto& layerProperties : availableLayers) {
			if (strcmp(layerName, layerProperties.layerName) == 0) {
				layerFound = true;
				break;
			}
		}

		if (!layerFound) {
			return false;
		}
	}

	return true;
}

const std::vector<const char*> HelloTriangleApplication::m_DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<Vertex> HelloTriangleApplication::m_Vertices = {
	// Top
	{ { -0.5f, -0.5,  0.5f },{ 0.0f, 0.0f, 1.0f }, { 1.0f, 0.0f }}, 
	{ {  0.5f, -0.5,  0.5f },{ 1.0f, 0.0f, 1.0f }, { 0.0f, 0.0f }}, 
	{ {  0.5f,  0.5,  0.5f },{ 1.0f, 1.0f, 1.0f }, { 0.0f, 1.0f }},  
	{ { -0.5f,  0.5,  0.5f },{ 0.0f, 1.0f, 1.0f }, { 1.0f, 1.0f }}, 
	// Left
	{ { -0.5f, -0.5, -0.5f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
	{ { -0.5f, -0.5,  0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },
	{ { -0.5f,  0.5,  0.5f },{ 0.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } },
	{ { -0.5f,  0.5, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
	// Front
	{ { -0.5f,  0.5, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	{ { -0.5f,  0.5,  0.5f },{ 0.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
	{ {  0.5f,  0.5,  0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 1.0f } } ,
	{ {  0.5f,  0.5, -0.5f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
	// Right
	{ {  0.5f,  0.5, -0.5f },{ 1.0f, 1.0f, 0.0f },{ 1.0f, 0.0f } },
	{ {  0.5f,  0.5,  0.5f },{ 1.0f, 1.0f, 1.0f },{ 0.0f, 0.0f } },
	{ {  0.5f, -0.5,  0.5f },{ 1.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
	{ {  0.5f, -0.5, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
	// Back
	{ {  0.5f, -0.5, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } },
	{ {  0.5f, -0.5,  0.5f },{ 1.0f, 0.0f, 1.0f },{ 0.0f, 0.0f } },
	{ { -0.5f, -0.5,  0.5f },{ 0.0f, 0.0f, 1.0f },{ 0.0f, 1.0f } },
	{ { -0.5f, -0.5, -0.5f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 1.0f } },
	// Bottom 
	{ { -0.5f,  0.5, -0.5f },{ 0.0f, 1.0f, 0.0f },{ 1.0f, 1.0f } },
	{ {  0.5f,  0.5, -0.5f },{ 1.0f, 1.0f, 0.0f },{ 0.0f, 1.0f } },
	{ {  0.5f, -0.5, -0.5f },{ 1.0f, 0.0f, 0.0f },{ 0.0f, 0.0f } },
	{ { -0.5f, -0.5, -0.5f },{ 0.0f, 0.0f, 0.0f },{ 1.0f, 0.0f } }
};

const std::vector<uint16_t>HelloTriangleApplication::m_Indices = {
	4 * 0 + 0, 4 * 0 + 1, 4 * 0 + 2, 4 * 0 + 2, 4 * 0 + 3, 4 * 0 + 0, // Top
	4 * 1 + 0, 4 * 1 + 1, 4 * 1 + 2, 4 * 1 + 2, 4 * 1 + 3, 4 * 1 + 0, // Left
	4 * 2 + 0, 4 * 2 + 1, 4 * 2 + 2, 4 * 2 + 2, 4 * 2 + 3, 4 * 2 + 0, // Front
	4 * 3 + 0, 4 * 3 + 1, 4 * 3 + 2, 4 * 3 + 2, 4 * 3 + 3, 4 * 3 + 0, // Right
	4 * 4 + 0, 4 * 4 + 1, 4 * 4 + 2, 4 * 4 + 2, 4 * 4 + 3, 4 * 4 + 0, // Back
	4 * 5 + 0, 4 * 5 + 1, 4 * 5 + 2, 4 * 5 + 2, 4 * 5 + 3, 4 * 5 + 0  // Bottom
};

HelloTriangleApplication::HelloTriangleApplication(Scene scene) 
	: m_Window(GetModuleHandle(nullptr), "VulkanTest", "Vulkan Test", 1, WIDTH, HEIGHT, false), 
	  m_Scene(scene)
{
	m_Window.GetHandle(); // Actually intializes the window
}

void HelloTriangleApplication::createVertexBuffer()
{
	auto buffer_size = sizeof(Vertex)*m_Vertices.size();
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = buffer_size;
	buffer_create_info.flags = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	auto buffer = Buffer(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
	
	buffer.map();
	memcpy(buffer.mappedMemory(), m_Vertices.data(), buffer_size);
	buffer.unmap();

	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	m_VertexBuffer = std::make_unique<Buffer>(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	copyBuffer(buffer.m_Buffer, m_VertexBuffer->m_Buffer, buffer_size);
}

void HelloTriangleApplication::createIndexBuffer()
{
	auto buffer_size = sizeof m_Indices[0]*m_Indices.size();
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = buffer_size;
	buffer_create_info.flags = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	auto buffer = Buffer(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	buffer.map();
	memcpy(buffer.mappedMemory(), m_Indices.data(), buffer_size);
	buffer.unmap();

	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	m_IndexBuffer =  std::make_unique<Buffer>(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

	copyBuffer(buffer.m_Buffer, m_IndexBuffer->m_Buffer, buffer_size);
}

void HelloTriangleApplication::createDescriptorSetLayout()
{
	/*
	 * Layout bindings
	 * 0 : uniform buffer object layout
	 * 1 : dynamic uniform buffer object layout
	 * 2 : sampler layout
	 */
	std::array<VkDescriptorSetLayoutBinding, 3> bindings = {};

	bindings[0].binding = 0;
	bindings[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[0].pImmutableSamplers = nullptr; // optional

	bindings[1].binding = 1;
	bindings[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
	bindings[1].pImmutableSamplers = nullptr; // optional

	bindings[2].binding = 2;
	bindings[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	bindings[2].descriptorCount = 1;
	bindings[2].stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT;
	bindings[2].pImmutableSamplers = nullptr; // optional

	VkDescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();

	if(vkCreateDescriptorSetLayout(m_LogicalDevice, &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create discriptor set layout!");
	}
}

void HelloTriangleApplication::createUniformBuffer()
{
	VkPhysicalDeviceProperties properties;
	vkGetPhysicalDeviceProperties(m_PhysicalDevice, &properties);

	auto buffer_size = sizeof(m_UniformBufferObject);
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = buffer_size;
	buffer_create_info.flags = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	m_UniformBuffer = std::make_unique<Buffer>(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	auto allignment = properties.limits.minUniformBufferOffsetAlignment;
	m_DynamicAllignment = sizeof(*m_InstanceUniformBufferObject.model);

	if(allignment > 0)
	{
		m_DynamicAllignment = (m_DynamicAllignment + allignment - 1) & ~(allignment - 1);
	}

	buffer_size = m_Scene.renderObjects().size() * m_DynamicAllignment;
	m_InstanceUniformBufferObject.model = static_cast<glm::mat4 *>(_aligned_malloc(buffer_size, m_DynamicAllignment));
	buffer_create_info.size = buffer_size;
	buffer_create_info.usage = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
	// Because no HOST_COHERENT flag we must flush the buffer when writing to it
	m_DynamicUniformBuffer = std::make_unique<Buffer>(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT); 
	m_DynamicUniformBuffer->map();
}


#define E4 329
#define G4 392
#define C5 523

VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(
	VkDebugReportFlagsEXT flags, 
	VkDebugReportObjectTypeEXT objType, 
	uint64_t obj, 
	size_t location, 
	int32_t code, 
	const char* layerPrefix, 
	const char* msg, 
	void* userdata)
{
	std::cerr << "Validation Layer: " << msg << std::endl;
	return VK_FALSE;
}

void HelloTriangleApplication::setupDebugCallback()
{
	if (!enableValidationLayers) return;

	VkDebugReportCallbackCreateInfoEXT createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_REPORT_CALLBACK_CREATE_INFO_EXT;
	createInfo.flags = VK_DEBUG_REPORT_ERROR_BIT_EXT | VK_DEBUG_REPORT_WARNING_BIT_EXT;
	createInfo.pfnCallback = debugCallback;

	if (CreateDebugReportCallbackEXT(m_Instance, &createInfo, nullptr, &m_Callback) != VK_SUCCESS) {
		throw std::runtime_error("Failed to set up debug callback!");
	}
}

void HelloTriangleApplication::createDescriptorPool()
{
	std::array<VkDescriptorPoolSize, 3> pool_sizes;
	pool_sizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	pool_sizes[0].descriptorCount = 1;
	pool_sizes[1].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	pool_sizes[1].descriptorCount = 1;
	pool_sizes[2].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	pool_sizes[2].descriptorCount = 1;

	VkDescriptorPoolCreateInfo pool_info = {};
	pool_info.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
	pool_info.poolSizeCount = pool_sizes.size();
	pool_info.pPoolSizes = pool_sizes.data();
	pool_info.maxSets = 1; // TODO: Change to two ??

	if(vkCreateDescriptorPool(m_LogicalDevice, &pool_info, nullptr, &m_DescriptorPool) != VK_SUCCESS)
	{
		std::runtime_error("Failed to create descriptor pool!");
	}
}

void HelloTriangleApplication::createDescriptorSet()
{
	VkDescriptorSetAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &m_DescriptorSetLayout;

	if (vkAllocateDescriptorSets(m_LogicalDevice, &allocInfo, &m_DescriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = m_UniformBuffer->m_Buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = m_UniformBuffer->size();

	VkDescriptorBufferInfo dynamicBufferInfo;
	dynamicBufferInfo.buffer = m_DynamicUniformBuffer->m_Buffer;
	dynamicBufferInfo.offset = 0;
	dynamicBufferInfo.range = m_DynamicAllignment;

	VkDescriptorImageInfo image_info;
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.imageView = m_TextureImage->m_ImageView;
	image_info.sampler = m_TextureSampler;

	std::array<VkWriteDescriptorSet, 3> descriptorWrites = {};
	descriptorWrites[0].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[0].dstSet = m_DescriptorSet;
	descriptorWrites[0].dstBinding = 0;
	descriptorWrites[0].dstArrayElement = 0;
	descriptorWrites[0].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
	descriptorWrites[0].descriptorCount = 1;
	descriptorWrites[0].pBufferInfo = &bufferInfo;
	descriptorWrites[0].pImageInfo = nullptr; 
	descriptorWrites[0].pTexelBufferView = nullptr; 

	descriptorWrites[1].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[1].dstSet = m_DescriptorSet;
	descriptorWrites[1].dstBinding = 1;
	descriptorWrites[1].dstArrayElement = 0;
	descriptorWrites[1].descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER_DYNAMIC;
	descriptorWrites[1].descriptorCount = 1;
	descriptorWrites[1].pBufferInfo = &dynamicBufferInfo;
	descriptorWrites[1].pImageInfo = nullptr; 
	descriptorWrites[1].pTexelBufferView = nullptr; 

	descriptorWrites[2].sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
	descriptorWrites[2].dstSet = m_DescriptorSet;
	descriptorWrites[2].dstBinding = 2;
	descriptorWrites[2].dstArrayElement = 0;
	descriptorWrites[2].descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
	descriptorWrites[2].descriptorCount = 1;
	descriptorWrites[2].pBufferInfo = nullptr;
	descriptorWrites[2].pImageInfo = &image_info; 
	descriptorWrites[2].pTexelBufferView = nullptr; 

	vkUpdateDescriptorSets(m_LogicalDevice, descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

VkImageView HelloTriangleApplication::createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags) const
{
	VkImageViewCreateInfo view_info = {};
	view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
	view_info.image = image;
	view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
	view_info.format = format;
	view_info.subresourceRange.aspectMask = aspect_flags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	VkImageView image_view;

	if (vkCreateImageView(m_LogicalDevice, &view_info, nullptr, &image_view) != VK_SUCCESS) {
		throw std::runtime_error("Failed to create image view!");
	}

	return image_view;
}

void HelloTriangleApplication::createTextureSampler()
{
	VkSamplerCreateInfo sampler_create_info = {};
	sampler_create_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
	sampler_create_info.magFilter = VK_FILTER_LINEAR;
	sampler_create_info.minFilter = VK_FILTER_LINEAR;
	sampler_create_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
	sampler_create_info.anisotropyEnable = VK_TRUE;
	sampler_create_info.maxAnisotropy = 16;
	sampler_create_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
	sampler_create_info.unnormalizedCoordinates = VK_FALSE;
	sampler_create_info.compareEnable = VK_FALSE;
	sampler_create_info.compareOp = VK_COMPARE_OP_ALWAYS;
	sampler_create_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.minLod = 0.0f;
	sampler_create_info.maxLod = 0.0f;

	if (vkCreateSampler(m_LogicalDevice, &sampler_create_info, nullptr, &m_TextureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

VkFormat HelloTriangleApplication::findSupportedFormat(const std::vector<VkFormat>& candidates, VkImageTiling tiling, VkFormatFeatureFlags features) const
{
	for(auto& format : candidates)
	{
		VkFormatProperties properties;
		vkGetPhysicalDeviceFormatProperties(m_PhysicalDevice, format, &properties);

		if ((tiling == VK_IMAGE_TILING_LINEAR  && (properties.linearTilingFeatures & features)  == features) || 
			(tiling == VK_IMAGE_TILING_OPTIMAL && (properties.optimalTilingFeatures & features) == features)) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

VkFormat HelloTriangleApplication::findDepthFormat() const
{
	return findSupportedFormat(
		{ VK_FORMAT_D32_SFLOAT, VK_FORMAT_D32_SFLOAT_S8_UINT, VK_FORMAT_D24_UNORM_S8_UINT },
		VK_IMAGE_TILING_OPTIMAL,
		VK_FORMAT_FEATURE_DEPTH_STENCIL_ATTACHMENT_BIT
	);
}

inline bool hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void HelloTriangleApplication::createDepthResources()
{
	auto depth_format = findDepthFormat();

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = static_cast<uint32_t>(m_SwapChainExtent.width);
	imageCreateInfo.extent.height = static_cast<uint32_t>(m_SwapChainExtent.height);
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = depth_format;
	imageCreateInfo.tiling = VK_IMAGE_TILING_OPTIMAL;
	imageCreateInfo.usage = VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;

	m_DepthImage = std::make_unique<Image>(m_LogicalDevice, m_PhysicalDevice, imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_DEPTH_BIT);

	transitionImageLayout(m_DepthImage->m_Image, m_DepthImage->m_Format, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

// Initializes Vulkan
void HelloTriangleApplication::initVulkan() {
	createInstance();
	setupDebugCallback();
	createSurface();
	pickPhysicalDevice();
	createLogicalDevice();
	createSwapChain();
	createImageViews();
	createRenderPass();
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool();
	createDepthResources();
	createFramebuffers();
	createTextureImage();
	createTextureSampler();
	createVertexBuffer();
	createIndexBuffer();
	createUniformBuffer();
	createDescriptorPool();
	createDescriptorSet();
	createCommandBuffers();
	createSemaphores();
}

void HelloTriangleApplication::createSemaphores() {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_ImageAvaliableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(m_LogicalDevice, &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS)
	{
		throw std::runtime_error("failed to create semaphores!");
	}
}

 void HelloTriangleApplication::createCommandBuffers() {
	m_CommandBuffers.resize(m_SwapChainFramebuffers.size());

	//allocate room for buffers in command pool:

	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY; //buffers can be primary (called to by user) or secondary (called to by primary buffer)
	allocInfo.commandBufferCount = static_cast<uint32_t>(m_CommandBuffers.size());

	if (vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate command buffers!");
	}

	//begin recording process:
	for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_SIMULTANEOUS_USE_BIT;
		beginInfo.pInheritanceInfo = nullptr;

		vkBeginCommandBuffer(m_CommandBuffers[i], &beginInfo);

		//starting render pass:
		VkRenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = { 0,0 };
		renderPassInfo.renderArea.extent = m_SwapChainExtent;

		std::array<VkClearValue, 2> clear_values = {};
		clear_values[0].color = {0.2f, 0.3f, 0.8f, 1.0f};
		clear_values[1].depthStencil = {1.0f, 0};

		renderPassInfo.clearValueCount = clear_values.size();
		renderPassInfo.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(
			m_CommandBuffers[i],	//<-- the buffer to record to
			&renderPassInfo,	//<-- the above info
			VK_SUBPASS_CONTENTS_INLINE	//<-- executed on primary buffer (as opposed to secondary buffer)
		);

		//bind graphics pipeline
		vkCmdBindPipeline(
			m_CommandBuffers[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS, //graphics pipeline, NOT compute pipeline
			m_GraphicsPipeline
		);


		VkDeviceSize offset =  0;
		vkCmdBindVertexBuffers(m_CommandBuffers[i], 0, 1, &m_VertexBuffer->m_Buffer, &offset);
		vkCmdBindIndexBuffer(m_CommandBuffers[i], m_IndexBuffer->m_Buffer, 0, VK_INDEX_TYPE_UINT16);

		for (int j = 0; j < m_Scene.renderObjects().size(); j++) {
			uint32_t dynamic_offset = j * m_DynamicAllignment;
			vkCmdBindDescriptorSets(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, m_PipelineLayout, 0, 1, &m_DescriptorSet, 1, &dynamic_offset);

			vkCmdDrawIndexed(
				m_CommandBuffers[i],
				m_Indices.size(),
				1,	//<-- NOT instanced rendering
				0,	//<-- first index (i.e. offset in indexbuffer)
				0,
				0
			);
		}

		//end recording
		vkCmdEndRenderPass(m_CommandBuffers[i]);

		if (vkEndCommandBuffer(m_CommandBuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to record command buffer!");
		}
	}
}

 void HelloTriangleApplication::createCommandPool() {
	QueueFamilyIndices queueFamilyIndices = findQueueFamilies(m_PhysicalDevice);

	VkCommandPoolCreateInfo poolInfo = {};
	poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
	poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily;
	poolInfo.flags = 0; //optional about rerecording strategy of cmd buffer(s)

	if (vkCreateCommandPool(m_LogicalDevice, &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

 void HelloTriangleApplication::createFramebuffers() {
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
		std::array<VkImageView, 2> attachments = {
			m_SwapChainImageViews[i],
			m_DepthImage->m_ImageView
		};

		VkFramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		if (vkCreateFramebuffer(m_LogicalDevice, &framebufferInfo, nullptr, &m_SwapChainFramebuffers[i]) != VK_SUCCESS) {
			throw std::runtime_error("failed to create framebuffer!");
		}
	}
}

 void HelloTriangleApplication::createRenderPass() {

	// Color buffer resides as swapchain image. 
	VkAttachmentDescription colorAttatchment = {};
	colorAttatchment.format = m_SwapChainImageFormat;
	colorAttatchment.samples = VK_SAMPLE_COUNT_1_BIT;
	colorAttatchment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear buffer data at load
	colorAttatchment.storeOp = VK_ATTACHMENT_STORE_OP_STORE; // Remember buffer data after store
	colorAttatchment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	colorAttatchment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	colorAttatchment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	colorAttatchment.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR; // Image is presented in swap chain

	VkAttachmentReference colorAttatchmentRef;
	colorAttatchmentRef.attachment = 0; // Since we only have one attatchment
	colorAttatchmentRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;


	// Depth buffer resides as swapchain image. 
	VkAttachmentDescription depthAttatchment = {};
	depthAttatchment.format = findDepthFormat();
	depthAttatchment.samples = VK_SAMPLE_COUNT_1_BIT;
	depthAttatchment.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR; // Clear buffer data at load
	depthAttatchment.storeOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttatchment.stencilLoadOp = VK_ATTACHMENT_LOAD_OP_DONT_CARE;
	depthAttatchment.stencilStoreOp = VK_ATTACHMENT_STORE_OP_DONT_CARE;
	depthAttatchment.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	depthAttatchment.finalLayout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL; 

	VkAttachmentReference depthAttatchmentRef;
	depthAttatchmentRef.attachment = 1; // Since we only have one attatchment
	depthAttatchmentRef.layout = VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL;

	VkSubpassDescription subpass = {};
	subpass.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
	subpass.colorAttachmentCount = 1;
	subpass.pColorAttachments = &colorAttatchmentRef;
	subpass.pDepthStencilAttachment = &depthAttatchmentRef;

	// Handling subpass dependencies
	VkSubpassDependency dependency = {};
	dependency.srcSubpass = VK_SUBPASS_EXTERNAL; // implicit previous subpass
	dependency.dstSubpass = 0; // Index of our subpass

							   // depend on color attatchment output stage
	dependency.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.srcAccessMask = 0;

	dependency.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
	dependency.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_READ_BIT | VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

	std::array<VkAttachmentDescription, 2> attachments = { colorAttatchment, depthAttatchment };
	VkRenderPassCreateInfo renderPassInfo = {};
	renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
	renderPassInfo.attachmentCount = attachments.size();
	renderPassInfo.pAttachments = attachments.data();
	renderPassInfo.subpassCount = 1;
	renderPassInfo.pSubpasses = &subpass;
	renderPassInfo.dependencyCount = 1;
	renderPassInfo.pDependencies = &dependency;

	if (vkCreateRenderPass(m_LogicalDevice, &renderPassInfo, nullptr, &m_RenderPass) != VK_SUCCESS) {
		throw std::runtime_error("failed to create render pass!");
	}
}

 void HelloTriangleApplication::createGraphicsPipeline() {

	//get byte code of shaders
	auto vertShader = Shader(m_LogicalDevice, "./shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	auto fragShader = Shader(m_LogicalDevice, "./shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

	//for later reference:
	VkPipelineShaderStageCreateInfo shaderStages[] = { vertShader.m_Info, fragShader.m_Info };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attribute_descriptions = Vertex::getAttributeDescriptions();

	// Information on how to read from vertex buffer
	VkPipelineVertexInputStateCreateInfo vertexInputInfo = {};
	vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
	vertexInputInfo.vertexBindingDescriptionCount = 1;
	vertexInputInfo.vertexAttributeDescriptionCount = attribute_descriptions.size();
	vertexInputInfo.pVertexBindingDescriptions = &bindingDescription;
	vertexInputInfo.pVertexAttributeDescriptions = attribute_descriptions.data();

	VkPipelineInputAssemblyStateCreateInfo inputAssembly = {};
	inputAssembly.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
	inputAssembly.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
	inputAssembly.primitiveRestartEnable = VK_FALSE;

	// Creating a viewport to render to
	VkViewport viewport;
	viewport.x = 0.0f;
	viewport.y = 0.0f;
	viewport.width = static_cast<float>(m_SwapChainExtent.width);
	viewport.height = static_cast<float>(m_SwapChainExtent.height);
	viewport.minDepth = 0.0f; // What is?
	viewport.maxDepth = 1.0f;

	// Scissor rectangle. Defines image cropping of viewport.
	VkRect2D scissor;
	scissor.offset = { 0, 0 }; // xy
	scissor.extent = m_SwapChainExtent; // width height

									  // Combine viewport and scissor into a viewport state
	VkPipelineViewportStateCreateInfo viewportState = {};
	viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
	viewportState.viewportCount = 1;
	viewportState.pViewports = &viewport;
	viewportState.scissorCount = 1;
	viewportState.pScissors = &scissor;

	VkPipelineRasterizationStateCreateInfo rasterizer = {};
	rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
	rasterizer.depthClampEnable = VK_FALSE;
	rasterizer.rasterizerDiscardEnable = VK_FALSE;
	rasterizer.polygonMode = VK_POLYGON_MODE_FILL;
	rasterizer.lineWidth = 1.0f;
	rasterizer.cullMode = VK_CULL_MODE_BACK_BIT;
	rasterizer.frontFace = VK_FRONT_FACE_COUNTER_CLOCKWISE;
	rasterizer.depthBiasEnable = VK_FALSE;
	rasterizer.depthBiasConstantFactor = 0.0f; //optional
	rasterizer.depthBiasClamp = 0.0f; //optional
	rasterizer.depthBiasSlopeFactor = 0.0f; //optional

											// Multisampling. Not in use but works to do anti aliasing
	VkPipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
	multisampling.sampleShadingEnable = VK_FALSE;
	multisampling.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
	multisampling.minSampleShading = 1.0f; //optional
	multisampling.pSampleMask = nullptr; //optional
	multisampling.alphaToCoverageEnable = VK_FALSE; //optional
	multisampling.alphaToOneEnable = VK_FALSE; //optional

											   //Color blending
	VkPipelineColorBlendAttachmentState colorBlendAttatchment;
	colorBlendAttatchment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
	colorBlendAttatchment.blendEnable = VK_FALSE; // TODO: Why false? Try setting to true.
	colorBlendAttatchment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;  // optional
	colorBlendAttatchment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	colorBlendAttatchment.colorBlendOp = VK_BLEND_OP_ADD; // optional
	colorBlendAttatchment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // optional
	colorBlendAttatchment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // optional
	colorBlendAttatchment.alphaBlendOp = VK_BLEND_OP_ADD; // optional

														  //Global Color Blending
	VkPipelineColorBlendStateCreateInfo	colorBlending = {};
	colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
	colorBlending.logicOpEnable = VK_FALSE;
	colorBlending.logicOp = VK_LOGIC_OP_COPY; // optional
	colorBlending.attachmentCount = 1;
	colorBlending.pAttachments = &colorBlendAttatchment;
	colorBlending.blendConstants[0] = 0.0f; // optional
	colorBlending.blendConstants[1] = 0.0f; // optional
	colorBlending.blendConstants[2] = 0.0f; // optional
	colorBlending.blendConstants[3] = 0.0f; // optional

	 // For uniforms
	VkPipelineLayoutCreateInfo pipelineLayoutInfo = {};
	pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = &m_DescriptorSetLayout;
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	if (vkCreatePipelineLayout(m_LogicalDevice, &pipelineLayoutInfo, nullptr, &m_PipelineLayout) != VK_SUCCESS) {
		throw std::runtime_error("failed to create pipeline layout!");
	}

	VkPipelineDepthStencilStateCreateInfo depth_stencil_info = {};
	depth_stencil_info.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
	depth_stencil_info.depthTestEnable = VK_TRUE;
	depth_stencil_info.depthWriteEnable = VK_TRUE;
	depth_stencil_info.depthCompareOp = VK_COMPARE_OP_LESS;
	depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_info.minDepthBounds = 0.0f;
	depth_stencil_info.maxDepthBounds = 1.0f;
	depth_stencil_info.stencilTestEnable = VK_FALSE;
	depth_stencil_info.front = {};
	depth_stencil_info.back = {};

	//TODO: create actual pipeline
	VkGraphicsPipelineCreateInfo pipelineInfo = {};
	pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = &inputAssembly;
	pipelineInfo.pViewportState = &viewportState;
	pipelineInfo.pRasterizationState = &rasterizer;
	pipelineInfo.pMultisampleState = &multisampling;
	pipelineInfo.pDepthStencilState = &depth_stencil_info;
	pipelineInfo.pColorBlendState = &colorBlending;
	pipelineInfo.pDynamicState = nullptr; //optional

	pipelineInfo.layout = m_PipelineLayout;

	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;

	pipelineInfo.basePipelineHandle = VK_NULL_HANDLE; //optional // we do not use pipeline derivation
	pipelineInfo.basePipelineIndex = -1; //optional //see above

	VkResult result = vkCreateGraphicsPipelines(
		m_LogicalDevice,		//<-- device
		VK_NULL_HANDLE,		//<-- pipeline cache for creation on mult. pipelines
		1,					//<-- number of infos in array below
		&pipelineInfo,		//<-- pipeline info array
		nullptr,			//<-- allocator
		&m_GraphicsPipeline	//<-- output
	);

	if (result != VK_SUCCESS) {
		throw std::runtime_error("failed to create graphics pipeline!");
	}
}

 void HelloTriangleApplication::createImageViews() {
	m_SwapChainImageViews.resize(m_SwapChainImages.size());

	for (size_t i = 0; i < m_SwapChainImages.size(); i++) {
		m_SwapChainImageViews[i] = createImageView(m_SwapChainImages[i], m_SwapChainImageFormat);
	}
}

//  Creates and sets the swapchain + sets "swapChainImageFormat" and "swapChainExtent".
 void HelloTriangleApplication::createSwapChain() {
	SwapChainSupportDetails swapChainSupport = querySwapChainSupport(m_PhysicalDevice);

	VkSurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	VkPresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentmodes);
	VkExtent2D extent = chooseSwapExtend(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	VkSwapchainCreateInfoKHR createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
	createInfo.surface = m_Surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = extent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;

	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
	uint32_t queueFamilyIndices[] = {
		static_cast<uint32_t>(indices.graphicsFamily),
		static_cast<uint32_t>(indices.presentFamily)
	};

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = VK_SHARING_MODE_CONCURRENT;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
		createInfo.queueFamilyIndexCount = 0; //optional
		createInfo.pQueueFamilyIndices = nullptr; //optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	createInfo.oldSwapchain = VK_NULL_HANDLE;

	if (vkCreateSwapchainKHR(m_LogicalDevice, &createInfo, nullptr, &m_SwapChain) != VK_SUCCESS) {
		throw std::runtime_error("failed to create swap chain! :(");
	}

	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, nullptr);
	m_SwapChainImages.resize(imageCount);
	vkGetSwapchainImagesKHR(m_LogicalDevice, m_SwapChain, &imageCount, m_SwapChainImages.data());

	m_SwapChainImageFormat = surfaceFormat.format;
	m_SwapChainExtent = extent;
}

 void HelloTriangleApplication::createSurface() {
	 VkWin32SurfaceCreateInfoKHR surface_info = {};
	 surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	 surface_info.hwnd = m_Window.GetHandle();
	 surface_info.hinstance = GetModuleHandle(nullptr);

	 auto CreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(vkGetInstanceProcAddr(m_Instance, "vkCreateWin32SurfaceKHR"));

	 if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(m_Instance, &surface_info, nullptr, &m_Surface) != VK_SUCCESS) {
		 throw std::runtime_error("failed to create window surface!");
	 }
}

 void HelloTriangleApplication::createLogicalDevice() {
	auto indices = findQueueFamilies(m_PhysicalDevice);

	std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
	float queuePriorty = 1.0f;

	// Runs over each family and makes a createinfo object for them
	for (int queueFamily : uniqueQueueFamilies) {
		VkDeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriorty; // Priority required even with 1 queue
		queueCreateInfos.push_back(queueCreateInfo);
	}


	VkPhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;

												  // Creating VkDeviceCreateInfo object
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(m_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = m_DeviceExtensions.data();

	createInfo.enabledLayerCount = 0;

	if (vkCreateDevice(m_PhysicalDevice, &createInfo, nullptr, &m_LogicalDevice) != VK_SUCCESS) {
		throw std::runtime_error("failed to create logical device!");
	}

	// Get handle to queue in the logicalDevice
	vkGetDeviceQueue(
		m_LogicalDevice, //<-- handle for device 
		indices.graphicsFamily, //<-- family of queues to access
		0, //<-- index of specific queue
		&m_GraphicsQueue //<-- handle output 
	);

	vkGetDeviceQueue(m_LogicalDevice, indices.presentFamily, 0, &m_PresentQueue);

}

 void HelloTriangleApplication::pickPhysicalDevice() {
	uint32_t deviceCount = 0;

	vkEnumeratePhysicalDevices(
		m_Instance,
		&deviceCount,	//<-- output 
		nullptr	//<-- output
	);

	if (deviceCount == 0) {
		throw std::runtime_error("failed to find GPUs with Vulkan support!");
	}


	std::vector<VkPhysicalDevice> devices(deviceCount);
	vkEnumeratePhysicalDevices(m_Instance, &deviceCount, devices.data());

	int i = 0;
	bool physicalDeviceFound = false;
	while (i < deviceCount && !physicalDeviceFound) {
		auto& device = devices.at(i);
		physicalDeviceFound = isDeviceSuitable(device);

		if (physicalDeviceFound) {
			m_PhysicalDevice = device;
		}

		i++;
	}

	if (!m_PhysicalDevice) {
		throw std::runtime_error("failed to find suitable GPU!");
	}
}

 QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(const VkPhysicalDevice& device) const
 {
	QueueFamilyIndices indices;

	uint32_t queueFamilyCount = 0;
	vkGetPhysicalDeviceQueueFamilyProperties(
		device,
		&queueFamilyCount, //<-- output
		nullptr //<-- output
	);

	std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
	vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

	for (int i = 0; i < queueFamilyCount; i++) {
		auto queueFamily = queueFamilies.at(i);

		//check for graphics family
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
			indices.graphicsFamily = i;
		}

		//check for present family
		VkBool32 presentSupport = false;
		vkGetPhysicalDeviceSurfaceSupportKHR(device, i, m_Surface, &presentSupport);
		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}


		if (indices.isComplete()) {
			break;
		}
	}

	return indices;
}

 bool HelloTriangleApplication::isDeviceSuitable(const VkPhysicalDevice& device) const
 {

	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentmodes.empty();
	}

	VkPhysicalDeviceFeatures supportedFeatures;
	vkGetPhysicalDeviceFeatures(device, &supportedFeatures);

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

 bool HelloTriangleApplication::checkDeviceExtensionSupport(const VkPhysicalDevice& device)
 {
	uint32_t extensionCount;
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

	std::vector<VkExtensionProperties> avaliableExtensions(extensionCount);
	vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, avaliableExtensions.data());

	std::set<std::string> requiredExtensions(m_DeviceExtensions.begin(), m_DeviceExtensions.end());

	for (const auto& extension : avaliableExtensions) {
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

std::vector<const char*> getRequiredExtensions()
{
	std::vector<const char*> extensions = {
		VK_KHR_SURFACE_EXTENSION_NAME,
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME
	};

	if (enableValidationLayers) {
		extensions.push_back(VK_EXT_DEBUG_REPORT_EXTENSION_NAME);
	}

	return extensions;
}

 void HelloTriangleApplication::createInstance() {
	 if (enableValidationLayers && !checkValidationLayerSupport()) {
		 throw std::runtime_error("validation layers requested, but not available!");
	 }

	//VkApplicationInfo is technically optional, but can be used to optimize
	VkApplicationInfo appInfo = {};
	appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
	appInfo.pApplicationName = "Hello Triangle";
	appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.pEngineName = "No Engine";	 // :(
	appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
	appInfo.apiVersion = VK_API_VERSION_1_0;

	//not optional!
	VkInstanceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
	createInfo.pApplicationInfo = &appInfo;

	auto extensions = getRequiredExtensions();
	createInfo.enabledExtensionCount = extensions.size();
	createInfo.ppEnabledExtensionNames = extensions.data();

	if (enableValidationLayers) {
		createInfo.enabledLayerCount = validationLayers.size();
		createInfo.ppEnabledLayerNames = validationLayers.data();
	}
	else
	{
		createInfo.enabledLayerCount = 0;
	}

	 auto result = vkCreateInstance(
		&createInfo,	//<-- info filled in above
		nullptr,		//<-- callback for custom allocation
		&m_Instance);		//<-- handle to created instance

	if (result != VK_SUCCESS) {
		throw std::runtime_error("Failed to create instance.");
	}
}

 SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(const VkPhysicalDevice& device) const
 {
	SwapChainSupportDetails details;
	vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, m_Surface, &details.capabilities);

	uint32_t formatCount;
	vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, nullptr);

	if (formatCount != 0) {
		details.formats.resize(formatCount);
		vkGetPhysicalDeviceSurfaceFormatsKHR(device, m_Surface, &formatCount, details.formats.data());
	}

	uint32_t presentModeCount;
	vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, nullptr);

	if (presentModeCount != 0) {
		details.presentmodes.resize(presentModeCount);
		vkGetPhysicalDeviceSurfacePresentModesKHR(device, m_Surface, &presentModeCount, details.presentmodes.data());
	}

	return details;
}

 VkSurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(const std::vector<VkSurfaceFormatKHR>& availableFormats) {

	//In the case the surface has no preference
	if (availableFormats.size() == 1 && availableFormats[0].format == VK_FORMAT_UNDEFINED) {
		return{ VK_FORMAT_B8G8R8A8_UNORM, VK_COLORSPACE_SRGB_NONLINEAR_KHR };
	}

	// If we're not allowed to freely choose a format  
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == VK_FORMAT_B8G8R8A8_UNORM &&
			availableFormat.colorSpace == VK_COLORSPACE_SRGB_NONLINEAR_KHR)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

 VkPresentModeKHR HelloTriangleApplication::chooseSwapPresentMode(const std::vector<VkPresentModeKHR> availablePresentModes)
{
	VkPresentModeKHR bestMode = VK_PRESENT_MODE_FIFO_KHR;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == VK_PRESENT_MODE_MAILBOX_KHR) {
			return availablePresentMode;
		}
		if (availablePresentMode == VK_PRESENT_MODE_IMMEDIATE_KHR) {
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

 VkExtent2D HelloTriangleApplication::chooseSwapExtend(const VkSurfaceCapabilitiesKHR & capabilities) const
 {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}

	VkExtent2D actualExtent = { m_Window.width(), m_Window.height() };
	
	actualExtent.width = std::max(
		 capabilities.minImageExtent.width,
		 std::min(capabilities.maxImageExtent.width, actualExtent.width)
	);

	actualExtent.height = std::max(
		capabilities.minImageExtent.height,
		std::min(capabilities.maxImageExtent.height, actualExtent.height)
	);

	 return actualExtent;
}

glm::vec3 convertToGLM(const Vec4f& vec)
{
	return { vec.x, vec.y, vec.z };
}

void HelloTriangleApplication::updateUniformBuffer()
{
	m_UniformBufferObject.view = lookAt(
		convertToGLM(m_Scene.camera().Position()),
		convertToGLM(m_Scene.camera().Target()),
		convertToGLM(m_Scene.camera().Up()));
	m_UniformBufferObject.projection = glm::perspective(
		m_Scene.camera().FieldOfView(),
		m_Window.aspectRatio(),
		m_Scene.camera().Near(),
		m_Scene.camera().Far());
	m_UniformBufferObject.projection[1][1] *= -1; // flip up and down

	m_UniformBuffer->map();
	memcpy(m_UniformBuffer->mappedMemory(), &m_UniformBufferObject, sizeof(m_UniformBufferObject));
	m_UniformBuffer->unmap();
}

void HelloTriangleApplication::updateDynamicUniformBuffer() const
{
	for (auto index = 0; index < m_Scene.renderObjects().size(); index++)
	{
		auto& render_object = m_Scene.renderObjects()[index];
		auto model = reinterpret_cast<glm::mat4*>(reinterpret_cast<uint64_t>(m_InstanceUniformBufferObject.model) + (index * m_DynamicAllignment));
		*model = translate(glm::mat4(), { render_object.x(), render_object.y(), render_object.z() });
	}
	VkMappedMemoryRange mappedMemoryRange{};
	mappedMemoryRange.sType = VK_STRUCTURE_TYPE_MAPPED_MEMORY_RANGE;
	mappedMemoryRange.memory = m_DynamicUniformBuffer->m_Memory;
	mappedMemoryRange.size = m_DynamicAllignment * m_Scene.renderObjects().size();

	memcpy(m_DynamicUniformBuffer->mappedMemory(), m_InstanceUniformBufferObject.model, m_DynamicAllignment * m_Scene.renderObjects().size());
	vkFlushMappedMemoryRanges(m_LogicalDevice, 1, &mappedMemoryRange);
}

void HelloTriangleApplication::mainLoop() {

	while (true)
	{
		MSG message;
		if (PeekMessage(&message, nullptr, 0, 0, PM_REMOVE))
		{
			if (message.message == WM_QUIT)
				break;

			TranslateMessage(&message);
			DispatchMessage(&message);
		}
		else
		{
			updateUniformBuffer();

			updateDynamicUniformBuffer();

			drawFrame();
		}
	}
	vkDeviceWaitIdle(m_LogicalDevice);
}

void HelloTriangleApplication::drawFrame() {

	// Aquire image
	uint32_t imageIndex;
	auto result = vkAcquireNextImageKHR(
		m_LogicalDevice, //<-- device driver
		m_SwapChain, //<-- images to write to
		std::numeric_limits<uint64_t>::max(), //<-- disabled timeout
		m_ImageAvaliableSemaphore, // <--semaphore to use
		VK_NULL_HANDLE, // <-- fenches
		&imageIndex // <-- output. index of image fetched
	);

	if(result == VK_ERROR_OUT_OF_DATE_KHR)
	{
		recreateSwapChain();
		return;
	} 
	
	if(result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	//Submitting Command Buffer
	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;

	// Wait in this stage until semaphore is aquired
	VkSemaphore  waitSemaphores[] = { m_ImageAvaliableSemaphore };
	VkPipelineStageFlags waitStages[] = {
		VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
		//VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT 
	};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[imageIndex]; // Use command buffers for aquired image

															  // Specify which sempahore to signal once command buffers have been executed.
	VkSemaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	if (vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE) != VK_SUCCESS) {
		throw std::runtime_error("failed to submit draw command buffer!");
	}

	// Present the image presented
	VkPresentInfoKHR presentInfo = {};
	presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	VkSwapchainKHR swapChains[] = { m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageIndex;

	presentInfo.pResults = nullptr; // Would contain VK result for all images if more than 1

	result = vkQueuePresentKHR(m_PresentQueue, &presentInfo);

	if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR)
	{
		recreateSwapChain();
		return;
	}

	if (result != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	vkQueueWaitIdle(m_PresentQueue);
}

void HelloTriangleApplication::cleanup() {
	cleanupSwapChain();

	this->~HelloTriangleApplication(); // HACK: Ensures that the buffers are destroyed before the vulkan instance

	_aligned_free(m_InstanceUniformBufferObject.model);

	vkDestroySampler(m_LogicalDevice, m_TextureSampler, nullptr);

	vkDestroyDescriptorPool(m_LogicalDevice, m_DescriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(m_LogicalDevice, m_DescriptorSetLayout, nullptr);

	vkDestroySemaphore(m_LogicalDevice, m_RenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(m_LogicalDevice, m_ImageAvaliableSemaphore, nullptr);

	vkDestroyCommandPool(m_LogicalDevice, m_CommandPool, nullptr);

	vkDestroyDevice(m_LogicalDevice, nullptr);
	vkDestroySurfaceKHR(m_Instance, m_Surface, nullptr);
	DestroyDebugReportCallbackEXT(m_Instance, m_Callback, nullptr);
	//clean stuff regarding instance before instance itself
	vkDestroyInstance(m_Instance, nullptr);
}

void HelloTriangleApplication::recreateSwapChain()
{
	vkDeviceWaitIdle(m_LogicalDevice);

	cleanupSwapChain();

	createSwapChain();
	createImageViews();
	createRenderPass();
	createGraphicsPipeline();
	createDepthResources();
	createFramebuffers();
	createCommandBuffers();
}

void HelloTriangleApplication::cleanupSwapChain()
{
	vkDeviceWaitIdle(m_LogicalDevice);
	for (size_t i = 0; i < m_SwapChainFramebuffers.size(); i++) {
		vkDestroyFramebuffer(m_LogicalDevice, m_SwapChainFramebuffers[i], nullptr);
	}

	vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, m_CommandBuffers.size(), m_CommandBuffers.data());

	vkDestroyPipeline(m_LogicalDevice, m_GraphicsPipeline, nullptr);
	vkDestroyPipelineLayout(m_LogicalDevice, m_PipelineLayout, nullptr);
	vkDestroyRenderPass(m_LogicalDevice, m_RenderPass, nullptr);

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
		vkDestroyImageView(m_LogicalDevice, m_SwapChainImageViews[i], nullptr);
	}

	vkDestroySwapchainKHR(m_LogicalDevice, m_SwapChain, nullptr);
}

// TODO: Flyt denne metode ind i buffer klassen
void HelloTriangleApplication::copyBuffer(VkBuffer source, VkBuffer destination, VkDeviceSize size)
{
	auto commandBuffer = beginSingleTimeCommands();

	VkBufferCopy copyRegion;
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;
	vkCmdCopyBuffer(commandBuffer, source, destination, 1, &copyRegion);

	endSingleTimeCommands(commandBuffer);
}

void HelloTriangleApplication::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/sample_image.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}
	VkDeviceSize imageSize = texWidth * texHeight * 4;

	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = imageSize;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	Buffer buffer(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

	buffer.map();
	memcpy(buffer.mappedMemory(), pixels, static_cast<size_t>(imageSize));
	buffer.unmap();

	stbi_image_free(pixels);

	VkImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
	imageCreateInfo.imageType = VK_IMAGE_TYPE_2D;
	imageCreateInfo.extent.width = texWidth;
	imageCreateInfo.extent.height = texHeight;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = VK_FORMAT_R8G8B8A8_UNORM;
	imageCreateInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
	imageCreateInfo.usage = VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT;
	imageCreateInfo.samples = VK_SAMPLE_COUNT_1_BIT;
	m_TextureImage = std::make_unique<Image>(m_LogicalDevice, m_PhysicalDevice, imageCreateInfo, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, VK_IMAGE_ASPECT_COLOR_BIT);
	transitionImageLayout(m_TextureImage->m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(buffer.m_Buffer, m_TextureImage->m_Image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));

	//prepare to use image in shader:
	transitionImageLayout(m_TextureImage->m_Image, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkCommandBuffer HelloTriangleApplication::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(m_LogicalDevice, &allocInfo, &commandBuffer);

	VkCommandBufferBeginInfo beginInfo = {};
	beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
	beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

	vkBeginCommandBuffer(commandBuffer, &beginInfo);
	return commandBuffer;
}

void HelloTriangleApplication::endSingleTimeCommands(VkCommandBuffer commandBuffer)
{
	vkEndCommandBuffer(commandBuffer);

	VkSubmitInfo submitInfo = {};
	submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	vkQueueSubmit(m_GraphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
	vkQueueWaitIdle(m_GraphicsQueue);

	vkFreeCommandBuffers(m_LogicalDevice, m_CommandPool, 1, &commandBuffer);
}

void HelloTriangleApplication::transitionImageLayout(VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout)
{
	VkCommandBuffer commandBuffer = beginSingleTimeCommands();

	VkImageMemoryBarrier barrier = {};
	barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
	barrier.oldLayout = oldLayout;
	barrier.newLayout = newLayout;
	barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
	barrier.image = image;
	barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	barrier.subresourceRange.baseMipLevel = 0;
	barrier.subresourceRange.levelCount = 1;
	barrier.subresourceRange.baseArrayLayer = 0;
	barrier.subresourceRange.layerCount = 1;

	// Special case for depth buffer image
	if(newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL)
	{
		barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_DEPTH_BIT;
		if(hasStencilComponent(format))
		{
			barrier.subresourceRange.aspectMask |= VK_IMAGE_ASPECT_STENCIL_BIT;
		}
	}
	
	VkPipelineStageFlags sourceStage;
	VkPipelineStageFlags destinationStage;

	if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL && newLayout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL) {
		barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
		barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

		sourceStage = VK_PIPELINE_STAGE_TRANSFER_BIT;
		destinationStage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
	}
	else if (oldLayout == VK_IMAGE_LAYOUT_UNDEFINED && newLayout == VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL) {
		barrier.srcAccessMask = 0;
		barrier.dstAccessMask = VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_READ_BIT | VK_ACCESS_DEPTH_STENCIL_ATTACHMENT_WRITE_BIT;

		sourceStage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
		destinationStage = VK_PIPELINE_STAGE_EARLY_FRAGMENT_TESTS_BIT;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	vkCmdPipelineBarrier(
		commandBuffer,
		sourceStage,
		destinationStage,
		0,
		0,
		nullptr,
		0,
		nullptr,
		1, &barrier
	);


	endSingleTimeCommands(commandBuffer);
}

void HelloTriangleApplication::copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
{
	auto commandBuffer = beginSingleTimeCommands();

	VkBufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = { 0,0,0 };
	region.imageExtent = {
		width,
		height,
		1
	};

	vkCmdCopyBufferToImage(
		commandBuffer,
		buffer,
		image,
		VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,	//<-- what layout the image is using (previous transition assumed)
		1,
		&region
	);

	endSingleTimeCommands(commandBuffer);
}



