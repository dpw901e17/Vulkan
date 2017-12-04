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

void DestroyDebugReportCallbackEXT(VkInstance instance, VkDebugReportCallbackEXT callback, const VkAllocationCallbacks* pAllocator = nullptr) {
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

const std::vector<const char*> HelloTriangleApplication::s_DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

const std::vector<Vertex> HelloTriangleApplication::s_Vertices = {
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

const std::vector<uint16_t>HelloTriangleApplication::s_Indices = {
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

void HelloTriangleApplication::run()
{
	initVulkan();

	updateUniformBuffer();
	updateDynamicUniformBuffer();

	glm::mat4 model_view = m_UniformBufferObject.view * m_InstanceUniformBufferObject.model[0];
	glm::vec3 model_space = {0.0f, 0.0f, 0.0f};
	glm::vec3 world_space = model_view * glm::vec4(model_space, 1.0f);
	glm::vec3 camera_space = m_UniformBufferObject.projection * model_view * glm::vec4(model_space, 1.0f);

	std::cout << "Model space:  " << model_space << std::endl;
	std::cout << "World space:  " << world_space << std::endl;
	std::cout << "Camera space: " << camera_space << std::endl;

	mainLoop();
	cleanup();
}

void HelloTriangleApplication::createVertexBuffer()
{
	auto buffer_size = sizeof(Vertex)*s_Vertices.size();
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = buffer_size;
	buffer_create_info.flags = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	auto buffer = Buffer(vk::PhysicalDevice(m_PhysicalDevice), m_LogicalDevice, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	
	memcpy(buffer.map(), s_Vertices.data(), buffer_size);
	buffer.unmap();

	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;

	m_VertexBuffer = std::make_unique<Buffer>(vk::PhysicalDevice(m_PhysicalDevice), m_LogicalDevice, buffer_create_info, vk::MemoryPropertyFlagBits::eDeviceLocal);

	copyBuffer(static_cast<VkBuffer>(buffer.m_Buffer), static_cast<VkBuffer>(m_VertexBuffer->m_Buffer), buffer_size);
}

void HelloTriangleApplication::createIndexBuffer()
{
	auto buffer_size = sizeof s_Indices[0]*s_Indices.size();
	VkBufferCreateInfo buffer_create_info = {};
	buffer_create_info.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
	buffer_create_info.size = buffer_size;
	buffer_create_info.flags = VK_SHARING_MODE_EXCLUSIVE;
	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

	auto buffer = Buffer(static_cast<vk::PhysicalDevice>(m_PhysicalDevice), m_LogicalDevice, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	memcpy(buffer.map(), s_Indices.data(), buffer_size);
	buffer.unmap();

	buffer_create_info.usage = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT;

	m_IndexBuffer =  std::make_unique<Buffer>(static_cast<vk::PhysicalDevice>(m_PhysicalDevice), m_LogicalDevice, buffer_create_info, vk::MemoryPropertyFlagBits::eDeviceLocal);

	copyBuffer(static_cast<VkBuffer>(buffer.m_Buffer), static_cast<VkBuffer>(m_IndexBuffer->m_Buffer), buffer_size);
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

	if(vkCreateDescriptorSetLayout(static_cast<VkDevice>(m_LogicalDevice), &layoutInfo, nullptr, &m_DescriptorSetLayout) != VK_SUCCESS)
	{
		throw std::runtime_error("Failed to create discriptor set layout!");
	}
}

void HelloTriangleApplication::createUniformBuffer()
{
	vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties();

	auto buffer_size = sizeof(m_UniformBufferObject);
	vk::BufferCreateInfo buffer_create_info;
	buffer_create_info.size = buffer_size;
	buffer_create_info.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	m_UniformBuffer = std::make_unique<Buffer>(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent);

	auto allignment = properties.limits.minUniformBufferOffsetAlignment;
	m_DynamicAllignment = sizeof(*m_InstanceUniformBufferObject.model);

	if(allignment > 0)
	{
		m_DynamicAllignment = (m_DynamicAllignment + allignment - 1) & ~(allignment - 1);
	}

	buffer_size = m_Scene.renderObjects().size() * m_DynamicAllignment;
	m_InstanceUniformBufferObject.model = static_cast<glm::mat4 *>(_aligned_malloc(buffer_size, m_DynamicAllignment));
	buffer_create_info.size = buffer_size;
	// Because no HOST_COHERENT flag we must flush the buffer when writing to it
	m_DynamicUniformBuffer = std::make_unique<Buffer>(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
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

	auto result = CreateDebugReportCallbackEXT(static_cast<VkInstance>(m_Instance), &createInfo, nullptr, &m_Callback);
	if (result != VK_SUCCESS)
	{
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

	if(vkCreateDescriptorPool(static_cast<VkDevice>(m_LogicalDevice), &pool_info, nullptr, &m_DescriptorPool) != VK_SUCCESS)
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

	if (vkAllocateDescriptorSets(static_cast<VkDevice>(m_LogicalDevice), &allocInfo, &m_DescriptorSet) != VK_SUCCESS) {
		throw std::runtime_error("failed to allocate descriptor set!");
	}

	VkDescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = static_cast<VkBuffer>(m_UniformBuffer->m_Buffer);
	bufferInfo.offset = 0;
	bufferInfo.range = m_UniformBuffer->size();

	VkDescriptorBufferInfo dynamicBufferInfo;
	dynamicBufferInfo.buffer = static_cast<VkBuffer>(m_DynamicUniformBuffer->m_Buffer);
	dynamicBufferInfo.offset = 0;
	dynamicBufferInfo.range = m_DynamicAllignment;

	VkDescriptorImageInfo image_info;
	image_info.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
	image_info.imageView = static_cast<VkImageView>(m_TextureImage->m_ImageView);
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

	vkUpdateDescriptorSets(static_cast<VkDevice>(m_LogicalDevice), descriptorWrites.size(), descriptorWrites.data(), 0, nullptr);
}

vk::ImageView HelloTriangleApplication::createImageView(vk::Image image, vk::Format format, vk::ImageAspectFlags aspect_flags) const
{
	vk::ImageViewCreateInfo view_info = {};
	view_info.image = image;
	view_info.viewType = vk::ImageViewType::e2D;
	view_info.format = format;
	view_info.subresourceRange.aspectMask = aspect_flags;
	view_info.subresourceRange.baseMipLevel = 0;
	view_info.subresourceRange.levelCount = 1;
	view_info.subresourceRange.baseArrayLayer = 0;
	view_info.subresourceRange.layerCount = 1;

	vk::ImageView image_view = m_LogicalDevice.createImageView(view_info);

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

	if (vkCreateSampler(static_cast<VkDevice>(m_LogicalDevice), &sampler_create_info, nullptr, &m_TextureSampler) != VK_SUCCESS) {
		throw std::runtime_error("failed to create texture sampler!");
	}
}

vk::Format HelloTriangleApplication::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
{
	for(auto& format : candidates)
	{
		auto properties = m_PhysicalDevice.getFormatProperties(format);

		if ((tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features)  == features) || 
			(tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

vk::Format HelloTriangleApplication::findDepthFormat() const
{
	return findSupportedFormat(
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);
}

inline bool hasStencilComponent(VkFormat format)
{
	return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void HelloTriangleApplication::createDepthResources()
{
	auto depth_format = findDepthFormat();

	vk::ImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.extent.width = m_SwapChainExtent.width;
	imageCreateInfo.extent.height = m_SwapChainExtent.height;
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = depth_format;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;

	m_DepthImage = std::make_unique<Image>(m_LogicalDevice, m_PhysicalDevice, imageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageAspectFlagBits::eDepth);

	transitionImageLayout(static_cast<VkImage>(m_DepthImage->m_Image), static_cast<VkFormat>(m_DepthImage->m_Format), VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void HelloTriangleApplication::createQueryPool()
{
	vk::QueryPoolCreateInfo query_pool_create_info;
	query_pool_create_info
		.setQueryType(vk::QueryType::ePipelineStatistics)
		.setQueryCount(m_SwapChainFramebuffers.size())
		.setPipelineStatistics(
			vk::QueryPipelineStatisticFlagBits::eClippingInvocations |
			vk::QueryPipelineStatisticFlagBits::eClippingPrimitives |
			vk::QueryPipelineStatisticFlagBits::eComputeShaderInvocations |
			vk::QueryPipelineStatisticFlagBits::eFragmentShaderInvocations |
			vk::QueryPipelineStatisticFlagBits::eGeometryShaderInvocations |
			vk::QueryPipelineStatisticFlagBits::eInputAssemblyPrimitives |
			vk::QueryPipelineStatisticFlagBits::eTessellationControlShaderPatches |
			vk::QueryPipelineStatisticFlagBits::eGeometryShaderPrimitives |
			vk::QueryPipelineStatisticFlagBits::eInputAssemblyVertices |
			vk::QueryPipelineStatisticFlagBits::eTessellationEvaluationShaderInvocations |
			vk::QueryPipelineStatisticFlagBits::eVertexShaderInvocations);

	// Throws exception on fail
	m_QueryPool = m_LogicalDevice.createQueryPool(query_pool_create_info);
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
	createQueryPool();
	createCommandBuffers();
	createSemaphores();
}

void HelloTriangleApplication::createSemaphores() {
	VkSemaphoreCreateInfo semaphoreInfo = {};
	semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

	if (vkCreateSemaphore(static_cast<VkDevice>(m_LogicalDevice), &semaphoreInfo, nullptr, &m_ImageAvaliableSemaphore) != VK_SUCCESS ||
		vkCreateSemaphore(static_cast<VkDevice>(m_LogicalDevice), &semaphoreInfo, nullptr, &m_RenderFinishedSemaphore) != VK_SUCCESS)
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

	if (vkAllocateCommandBuffers(static_cast<VkDevice>(m_LogicalDevice), &allocInfo, m_CommandBuffers.data()) != VK_SUCCESS) {
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
		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChainFramebuffers[i];

		renderPassInfo.renderArea.offset = vk::Offset2D{ 0,0 };
		renderPassInfo.renderArea.extent = m_SwapChainExtent;

		std::array<vk::ClearValue, 2> clear_values = {};
		clear_values[0].color = vk::ClearColorValue(std::array<float, 4>{0.2f, 0.3f, 0.8f, 1.0f});
		clear_values[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

		renderPassInfo.clearValueCount = clear_values.size();
		renderPassInfo.pClearValues = clear_values.data();

		vkCmdBeginRenderPass(
			m_CommandBuffers[i],	//<-- the buffer to record to
			reinterpret_cast<VkRenderPassBeginInfo *>(&renderPassInfo),	//<-- the above info
			VK_SUBPASS_CONTENTS_INLINE	//<-- executed on primary buffer (as opposed to secondary buffer)
		);

		//bind graphics pipeline
		vkCmdBindPipeline(
			m_CommandBuffers[i],
			VK_PIPELINE_BIND_POINT_GRAPHICS, //graphics pipeline, NOT compute pipeline
			m_GraphicsPipeline
		);


		VkDeviceSize offset =  0;
		vk::CommandBuffer cmdbuf(m_CommandBuffers[i]);
		cmdbuf.bindVertexBuffers(0, 1, &m_VertexBuffer->m_Buffer, &offset);
		cmdbuf.bindIndexBuffer(m_IndexBuffer->m_Buffer, 0, vk::IndexType::eUint16);

		cmdbuf.beginQuery(m_QueryPool, i, vk::QueryControlFlags());

		for (int j = 0; j < m_Scene.renderObjects().size(); j++) {
			uint32_t dynamic_offset = j * m_DynamicAllignment;
			vkCmdBindDescriptorSets(m_CommandBuffers[i], VK_PIPELINE_BIND_POINT_GRAPHICS, static_cast<VkPipelineLayout>(m_PipelineLayout), 0, 1, &m_DescriptorSet, 1, &dynamic_offset);

			vkCmdDrawIndexed(
				m_CommandBuffers[i],
				s_Indices.size(),
				1,	//<-- NOT instanced rendering
				0,	//<-- first index (i.e. offset in indexbuffer)
				0,
				0
			);
		}

		cmdbuf.endQuery(m_QueryPool, i);

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

	if (vkCreateCommandPool(static_cast<VkDevice>(m_LogicalDevice), &poolInfo, nullptr, &m_CommandPool) != VK_SUCCESS) {
		throw std::runtime_error("failed to create command pool!");
	}
}

 void HelloTriangleApplication::createFramebuffers() {
	m_SwapChainFramebuffers.resize(m_SwapChainImageViews.size());

	for (size_t i = 0; i < m_SwapChainImageViews.size(); i++) {
		std::array<vk::ImageView, 2> attachments = {
			m_SwapChainImageViews[i],
			m_DepthImage->m_ImageView
		};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = static_cast<vk::RenderPass>(m_RenderPass);
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChainExtent.width;
		framebufferInfo.height = m_SwapChainExtent.height;
		framebufferInfo.layers = 1;

		m_SwapChainFramebuffers[i] = m_LogicalDevice.createFramebuffer(framebufferInfo);


	}
}

 void HelloTriangleApplication::createRenderPass() {

	// Color buffer resides as swapchain image. 
	VkAttachmentDescription colorAttatchment = {};
	colorAttatchment.format = static_cast<VkFormat>(m_SwapChainImageFormat);
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
	depthAttatchment.format = static_cast<VkFormat>(findDepthFormat());
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

	m_RenderPass = m_LogicalDevice.createRenderPass(renderPassInfo);
}

 void HelloTriangleApplication::createGraphicsPipeline() {

	//get byte code of shaders
	auto vertShader = Shader(static_cast<VkDevice>(m_LogicalDevice), "./shaders/vert.spv", VK_SHADER_STAGE_VERTEX_BIT);
	auto fragShader = Shader(static_cast<VkDevice>(m_LogicalDevice), "./shaders/frag.spv", VK_SHADER_STAGE_FRAGMENT_BIT);

	//for later reference:
	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShader.m_Info, fragShader.m_Info };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attribute_descriptions = Vertex::getAttributeDescriptions();

	// Information on how to read from vertex buffer
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo = {};
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
	viewport.width = m_SwapChainExtent.width;
	viewport.height = m_SwapChainExtent.height;
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
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setLayoutCount = 1;
	pipelineLayoutInfo.pSetLayouts = reinterpret_cast<vk::DescriptorSetLayout*>(&m_DescriptorSetLayout);
	pipelineLayoutInfo.pushConstantRangeCount = 0;
	pipelineLayoutInfo.pPushConstantRanges = nullptr;

	m_PipelineLayout = m_LogicalDevice.createPipelineLayout(pipelineLayoutInfo);

	vk::PipelineDepthStencilStateCreateInfo depth_stencil_info = {};
	depth_stencil_info.depthTestEnable = VK_TRUE;
	depth_stencil_info.depthWriteEnable = VK_TRUE;
	depth_stencil_info.depthCompareOp = vk::CompareOp::eLess;
	depth_stencil_info.depthBoundsTestEnable = VK_FALSE;
	depth_stencil_info.minDepthBounds = 0.0f;
	depth_stencil_info.maxDepthBounds = 1.0f;
	depth_stencil_info.stencilTestEnable = VK_FALSE;
	depth_stencil_info.front = vk::StencilOpState();
	depth_stencil_info.back = vk::StencilOpState();

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.stageCount = 2;
	pipelineInfo.pStages = shaderStages;

	pipelineInfo.pVertexInputState = &vertexInputInfo;
	pipelineInfo.pInputAssemblyState = reinterpret_cast<vk::PipelineInputAssemblyStateCreateInfo *>(&inputAssembly);
	pipelineInfo.pViewportState = reinterpret_cast<vk::PipelineViewportStateCreateInfo *>(&viewportState);
	pipelineInfo.pRasterizationState = reinterpret_cast<vk::PipelineRasterizationStateCreateInfo *>(&rasterizer);
	pipelineInfo.pMultisampleState = reinterpret_cast<vk::PipelineMultisampleStateCreateInfo *>(&multisampling);
	pipelineInfo.pDepthStencilState = &depth_stencil_info;
	pipelineInfo.pColorBlendState = reinterpret_cast<vk::PipelineColorBlendStateCreateInfo *>(&colorBlending);
	pipelineInfo.pDynamicState = nullptr; //optional

	pipelineInfo.layout = m_PipelineLayout;

	pipelineInfo.renderPass = m_RenderPass;
	pipelineInfo.subpass = 0;

	 auto result = vkCreateGraphicsPipelines(
		static_cast<VkDevice>(m_LogicalDevice),		//<-- device
		VK_NULL_HANDLE,		//<-- pipeline cache for creation on mult. pipelines
		1,					//<-- number of infos in array below
		reinterpret_cast<VkGraphicsPipelineCreateInfo *>(&pipelineInfo),		//<-- pipeline info array
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

	vk::SurfaceFormatKHR surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	vk::PresentModeKHR presentMode = chooseSwapPresentMode(swapChainSupport.presentmodes);
	m_SwapChainExtent = chooseSwapExtend(swapChainSupport.capabilities);

	uint32_t imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount) {
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo = {};
	createInfo.surface = m_Surface;
	createInfo.minImageCount = imageCount;
	createInfo.imageFormat = surfaceFormat.format;
	createInfo.imageColorSpace = surfaceFormat.colorSpace;
	createInfo.imageExtent = m_SwapChainExtent;
	createInfo.imageArrayLayers = 1;
	createInfo.imageUsage = vk::ImageUsageFlagBits::eColorAttachment;

	QueueFamilyIndices indices = findQueueFamilies(m_PhysicalDevice);
	uint32_t queueFamilyIndices[] = {
		static_cast<uint32_t>(indices.graphicsFamily),
		static_cast<uint32_t>(indices.presentFamily)
	};

	if (indices.graphicsFamily != indices.presentFamily) {
		createInfo.imageSharingMode = vk::SharingMode::eConcurrent;
		createInfo.queueFamilyIndexCount = 2;
		createInfo.pQueueFamilyIndices = queueFamilyIndices;
	}
	else {
		createInfo.imageSharingMode = vk::SharingMode::eExclusive;
		createInfo.queueFamilyIndexCount = 0; //optional
		createInfo.pQueueFamilyIndices = nullptr; //optional
	}

	createInfo.preTransform = swapChainSupport.capabilities.currentTransform;
	createInfo.compositeAlpha = vk::CompositeAlphaFlagBitsKHR::eOpaque;
	createInfo.presentMode = presentMode;
	createInfo.clipped = VK_TRUE;

	m_SwapChain = m_LogicalDevice.createSwapchainKHR(createInfo);


	m_SwapChainImages = m_LogicalDevice.getSwapchainImagesKHR(m_SwapChain);

	
	m_SwapChainImageFormat = surfaceFormat.format;
}

 void HelloTriangleApplication::createSurface() {
	 VkWin32SurfaceCreateInfoKHR surface_info = {};
	 surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	 surface_info.hwnd = m_Window.GetHandle();
	 surface_info.hinstance = GetModuleHandle(nullptr);

	 auto CreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(m_Instance.getProcAddr("vkCreateWin32SurfaceKHR"));

	 if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(static_cast<VkInstance>(m_Instance), &surface_info, nullptr, reinterpret_cast<VkSurfaceKHR*>(&m_Surface)) != VK_SUCCESS) {
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
	deviceFeatures.pipelineStatisticsQuery = VK_TRUE;

												  // Creating VkDeviceCreateInfo object
	VkDeviceCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;

	createInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
	createInfo.pQueueCreateInfos = queueCreateInfos.data();

	createInfo.pEnabledFeatures = &deviceFeatures;

	createInfo.enabledExtensionCount = static_cast<uint32_t>(s_DeviceExtensions.size());
	createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();

	createInfo.enabledLayerCount = 0;

	m_LogicalDevice = m_PhysicalDevice.createDevice(createInfo);

	// Get handle to queue in the logicalDevice
	m_GraphicsQueue = m_LogicalDevice.getQueue(indices.graphicsFamily, 0);
	m_PresentQueue = m_LogicalDevice.getQueue(indices.presentFamily, 0);
}

 void HelloTriangleApplication::pickPhysicalDevice() {	
	for (auto& device : m_Instance.enumeratePhysicalDevices()) {
		if(isDeviceSuitable(device)){
			m_PhysicalDevice = device;
			break;
		}
	}

	if (!m_PhysicalDevice) {
		throw std::runtime_error("failed to find suitable GPU!");
	}
}

 QueueFamilyIndices HelloTriangleApplication::findQueueFamilies(const vk::PhysicalDevice& device) const
 {
	QueueFamilyIndices indices;


	std::vector<vk::QueueFamilyProperties> queueFamilies = device.getQueueFamilyProperties();
	
	for (int i = 0; i < queueFamilies.size(); i++) {
		auto queueFamily = queueFamilies.at(i);

		//check for graphics family
		if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
			indices.graphicsFamily = i;
		}

		//check for present family
		VkBool32 presentSupport = device.getSurfaceSupportKHR(i, m_Surface);

		if (queueFamily.queueCount > 0 && presentSupport) {
			indices.presentFamily = i;
		}


		if (indices.isComplete()) {
			break;
		}
	}

	return indices;
}

 bool HelloTriangleApplication::isDeviceSuitable(const vk::PhysicalDevice& device) const
 {

	QueueFamilyIndices indices = findQueueFamilies(device);

	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		SwapChainSupportDetails swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentmodes.empty();
	}

	vk::PhysicalDeviceFeatures supportedFeatures = device.getFeatures();

	
	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

 bool HelloTriangleApplication::checkDeviceExtensionSupport(const vk::PhysicalDevice& device)
 {

	std::vector<vk::ExtensionProperties> avaliableExtensions = device.enumerateDeviceExtensionProperties();
	

	std::set<std::string> requiredExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());

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

	m_Instance = vk::createInstance(createInfo);
}

 SwapChainSupportDetails HelloTriangleApplication::querySwapChainSupport(const vk::PhysicalDevice& device) const
 {
	SwapChainSupportDetails details;
	details.capabilities = device.getSurfaceCapabilitiesKHR(m_Surface);

	details.formats = device.getSurfaceFormatsKHR(m_Surface);
	
	details.presentmodes = device.getSurfacePresentModesKHR(m_Surface);

	return details;
}

 vk::SurfaceFormatKHR HelloTriangleApplication::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats) {

	//In the case the surface has no preference
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined) {
		return{ vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
	}

	// If we're not allowed to freely choose a format  
	for (const auto& availableFormat : availableFormats) {
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
			availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

 vk::PresentModeKHR HelloTriangleApplication::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	vk::PresentModeKHR bestMode = vk::PresentModeKHR::eFifo;

	for (const auto& availablePresentMode : availablePresentModes) {
		if (availablePresentMode == vk::PresentModeKHR::eMailbox) {
			return availablePresentMode;
		}
		if (availablePresentMode == vk::PresentModeKHR::eImmediate) {
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

	memcpy(m_UniformBuffer->map(), &m_UniformBufferObject, sizeof(m_UniformBufferObject));
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

	memcpy(m_DynamicUniformBuffer->map(), m_InstanceUniformBufferObject.model, m_DynamicAllignment * m_Scene.renderObjects().size());
	m_DynamicUniformBuffer->unmap();
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
	vkDeviceWaitIdle(static_cast<VkDevice>(m_LogicalDevice));
}

void HelloTriangleApplication::drawFrame() {

	// Aquire image
	auto imageResult = m_LogicalDevice.acquireNextImageKHR(m_SwapChain, std::numeric_limits<uint64_t>::max(), static_cast<vk::Semaphore>(m_ImageAvaliableSemaphore), vk::Fence());
	
	if(imageResult.result  == vk::Result::eErrorOutOfDateKHR)
	{
		recreateSwapChain();
		return;
	} 
	
	if(imageResult.result != vk::Result::eSuccess && imageResult.result != vk::Result::eSuboptimalKHR)
	{
		throw std::runtime_error("Failed to acquire swap chain image!");
	}

	//Submitting Command Buffer
	vk::SubmitInfo submitInfo = {};

	// Wait in this stage until semaphore is aquired
	vk::Semaphore  waitSemaphores[] = { static_cast<vk::Semaphore>(m_ImageAvaliableSemaphore) };
	vk::PipelineStageFlags waitStages[] = {
		vk::PipelineStageFlagBits::eColorAttachmentOutput
	};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = reinterpret_cast<vk::CommandBuffer*>(&m_CommandBuffers[imageResult.value]); // Use command buffers for aquired image

															  // Specify which sempahore to signal once command buffers have been executed.
	vk::Semaphore signalSemaphores[] = { static_cast<vk::Semaphore > (m_RenderFinishedSemaphore) };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	m_GraphicsQueue.submit({ submitInfo }, vk::Fence());

	// Present the image presented
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageResult.value;

	presentInfo.pResults = nullptr; // Would contain VK result for all images if more than 1

	m_PresentQueue.presentKHR(presentInfo);
	m_PresentQueue.waitIdle();

	PipelineStatisticsResult queryResults;

	if(m_LogicalDevice.getQueryPoolResults(
		m_QueryPool, 
		imageResult.value, 
		1, 
		sizeof queryResults, 
		&queryResults, 
		sizeof uint64_t, 
		vk::QueryResultFlagBits::eWait | vk::QueryResultFlagBits::e64) != vk::Result::eSuccess)
	{
		throw std::runtime_error("Failed get the query results from the logical device");
	}
}

void HelloTriangleApplication::cleanup() {
	cleanupSwapChain();

	this->~HelloTriangleApplication(); // HACK: Ensures that the buffers are destroyed before the vulkan instance

	_aligned_free(m_InstanceUniformBufferObject.model);

	vkDestroySampler(static_cast<VkDevice>(m_LogicalDevice), m_TextureSampler, nullptr);

	vkDestroyDescriptorPool(static_cast<VkDevice>(m_LogicalDevice), m_DescriptorPool, nullptr);

	vkDestroyDescriptorSetLayout(static_cast<VkDevice>(m_LogicalDevice), m_DescriptorSetLayout, nullptr);

	vkDestroySemaphore(static_cast<VkDevice>(m_LogicalDevice), m_RenderFinishedSemaphore, nullptr);
	vkDestroySemaphore(static_cast<VkDevice>(m_LogicalDevice), m_ImageAvaliableSemaphore, nullptr);

	vkDestroyCommandPool(static_cast<VkDevice>(m_LogicalDevice), m_CommandPool, nullptr);

	m_LogicalDevice.destroyQueryPool(m_QueryPool);

	m_LogicalDevice.destroy();
	m_Instance.destroySurfaceKHR(m_Surface);
	DestroyDebugReportCallbackEXT(static_cast<VkInstance>(m_Instance), m_Callback);
	//clean stuff regarding instance before instance itself
	m_Instance.destroy();
}

void HelloTriangleApplication::recreateSwapChain()
{
	vkDeviceWaitIdle(static_cast<VkDevice>(m_LogicalDevice));

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
	m_LogicalDevice.waitIdle();
	
	for (auto frameBuffer  : m_SwapChainFramebuffers) {
		m_LogicalDevice.destroyFramebuffer(frameBuffer);
	}

	m_LogicalDevice.freeCommandBuffers(static_cast<vk::CommandPool>(m_CommandPool), m_CommandBuffers.size(), reinterpret_cast<vk::CommandBuffer*>(m_CommandBuffers.data()));

	vkDestroyPipeline(static_cast<VkDevice>(m_LogicalDevice), m_GraphicsPipeline, nullptr);

	m_LogicalDevice.destroyPipelineLayout(m_PipelineLayout);

	m_LogicalDevice.destroyRenderPass(m_RenderPass);

	for (auto imageView : m_SwapChainImageViews) {
		m_LogicalDevice.destroyImageView(imageView);
	}

	m_LogicalDevice.destroySwapchainKHR(m_SwapChain);
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

	Buffer buffer(m_PhysicalDevice, m_LogicalDevice, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible| vk::MemoryPropertyFlagBits::eHostCoherent);

	memcpy(buffer.map(), pixels, static_cast<size_t>(imageSize));
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
	m_TextureImage = std::make_unique<Image>(m_LogicalDevice, m_PhysicalDevice, imageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageAspectFlagBits::eColor);
	transitionImageLayout(static_cast<VkImage>(m_TextureImage->m_Image), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
	copyBufferToImage(static_cast<VkBuffer>(buffer.m_Buffer), static_cast<VkImage>(m_TextureImage->m_Image), texWidth, texHeight);

	//prepare to use image in shader:
	transitionImageLayout(static_cast<VkImage>(m_TextureImage->m_Image), VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);
}

VkCommandBuffer HelloTriangleApplication::beginSingleTimeCommands()
{
	VkCommandBufferAllocateInfo allocInfo = {};
	allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
	allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	VkCommandBuffer commandBuffer;
	vkAllocateCommandBuffers(static_cast<VkDevice>(m_LogicalDevice), &allocInfo, &commandBuffer);

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

	m_GraphicsQueue.submit({ submitInfo }, vk::Fence());
	m_GraphicsQueue.waitIdle();

	vkFreeCommandBuffers(static_cast<VkDevice>(m_LogicalDevice), m_CommandPool, 1, &commandBuffer);
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



