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
	: m_Window(GetModuleHandle(nullptr), "VulkanTest", "Vulkan Test", WIDTH, HEIGHT), 
	  m_Scene(scene),
	  m_Instance(),
	  m_Surface(createSurface(m_Window, m_Instance)),
	  m_Device(m_Instance, m_Surface),
	  m_SwapChain(m_Window, m_Surface, m_Device)
{
}

void HelloTriangleApplication::run()
{
	initVulkan();
#ifndef NDEBUG
	updateUniformBuffer();
	updateDynamicUniformBuffer();

	glm::mat4 model_view = m_UniformBufferObject.view * m_InstanceUniformBufferObject.model[0];
	glm::vec3 model_space = {0.0f, 0.0f, 0.0f};
	glm::vec3 world_space = model_view * glm::vec4(model_space, 1.0f);
	glm::vec3 camera_space = m_UniformBufferObject.projection * model_view * glm::vec4(model_space, 1.0f);

	std::cout << "Model space:  " << model_space << std::endl;
	std::cout << "World space:  " << world_space << std::endl;
	std::cout << "Camera space: " << camera_space << std::endl;
#endif
	mainLoop();
	cleanup();
}

void HelloTriangleApplication::createVertexBuffer()
{
	auto buffer_size = sizeof(Vertex)*s_Vertices.size();
	vk::BufferCreateInfo buffer_create_info = {};
	buffer_create_info.size = buffer_size;
	buffer_create_info.usage = vk::BufferUsageFlagBits::eTransferSrc;

	auto buffer = Buffer(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
	
	memcpy(buffer.map(), s_Vertices.data(), buffer_size);
	buffer.unmap();

	buffer_create_info.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eVertexBuffer;

	m_VertexBuffer = std::make_unique<Buffer>(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eDeviceLocal);

	copyBuffer(buffer.m_Buffer, m_VertexBuffer->m_Buffer, buffer_size);
}

void HelloTriangleApplication::createIndexBuffer()
{
	auto buffer_size = sizeof s_Indices[0]*s_Indices.size();
	vk::BufferCreateInfo buffer_create_info = {};
	buffer_create_info.size = buffer_size;
	buffer_create_info.usage = vk::BufferUsageFlagBits::eTransferSrc;

	auto buffer = Buffer(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	memcpy(buffer.map(), s_Indices.data(), buffer_size);
	buffer.unmap();

	buffer_create_info.usage = vk::BufferUsageFlagBits::eTransferDst | vk::BufferUsageFlagBits::eIndexBuffer;

	m_IndexBuffer =  std::make_unique<Buffer>(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eDeviceLocal);

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
	std::array<vk::DescriptorSetLayoutBinding, 3> bindings = {};

	bindings[0].binding = 0;
	bindings[0].descriptorType = vk::DescriptorType::eUniformBuffer;
	bindings[0].descriptorCount = 1;
	bindings[0].stageFlags = vk::ShaderStageFlagBits::eVertex;

	bindings[1].binding = 1;
	bindings[1].descriptorType = vk::DescriptorType::eUniformBufferDynamic;
	bindings[1].descriptorCount = 1;
	bindings[1].stageFlags = vk::ShaderStageFlagBits::eVertex;

	bindings[2].binding = 2;
	bindings[2].descriptorType = vk::DescriptorType::eCombinedImageSampler;
	bindings[2].descriptorCount = 1;
	bindings[2].stageFlags = vk::ShaderStageFlagBits::eFragment;

	vk::DescriptorSetLayoutCreateInfo layoutInfo = {};
	layoutInfo.bindingCount = bindings.size();
	layoutInfo.pBindings = bindings.data();

	m_DescriptorSetLayout = m_Device->createDescriptorSetLayout(layoutInfo);
}

void HelloTriangleApplication::createUniformBuffer()
{
	vk::PhysicalDeviceProperties properties = m_PhysicalDevice.getProperties();

	auto buffer_size = sizeof(m_UniformBufferObject);
	vk::BufferCreateInfo buffer_create_info;
	buffer_create_info.size = buffer_size;
	buffer_create_info.usage = vk::BufferUsageFlagBits::eUniformBuffer;
	m_UniformBuffer = std::make_unique<Buffer>(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible|vk::MemoryPropertyFlagBits::eHostCoherent);

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
	m_DynamicUniformBuffer = std::make_unique<Buffer>(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);
}

void HelloTriangleApplication::createDescriptorPool()
{
	std::array<vk::DescriptorPoolSize, 3> pool_sizes;
	pool_sizes[0].type = vk::DescriptorType::eUniformBuffer;
	pool_sizes[0].descriptorCount = 1;
	pool_sizes[1].type = vk::DescriptorType::eUniformBufferDynamic;
	pool_sizes[1].descriptorCount = 1;
	pool_sizes[2].type = vk::DescriptorType::eCombinedImageSampler;
	pool_sizes[2].descriptorCount = 1;

	vk::DescriptorPoolCreateInfo pool_info = {};
	pool_info.poolSizeCount = pool_sizes.size();
	pool_info.pPoolSizes = pool_sizes.data();
	pool_info.maxSets = 1; // TODO: Change to two ??

	m_DescriptorPool = m_Device->createDescriptorPool(pool_info);
}

void HelloTriangleApplication::createDescriptorSet()
{
	vk::DescriptorSetAllocateInfo allocInfo = {};
	allocInfo.descriptorPool = m_DescriptorPool;
	allocInfo.descriptorSetCount = 1;
	allocInfo.pSetLayouts = &m_DescriptorSetLayout;

	m_DescriptorSet = m_Device->allocateDescriptorSets(allocInfo)[0]; // WARN: Hard coded 0 value

	vk::DescriptorBufferInfo bufferInfo;
	bufferInfo.buffer = m_UniformBuffer->m_Buffer;
	bufferInfo.offset = 0;
	bufferInfo.range = m_UniformBuffer->size();

	vk::DescriptorBufferInfo dynamicBufferInfo;
	dynamicBufferInfo.buffer = m_DynamicUniformBuffer->m_Buffer;
	dynamicBufferInfo.offset = 0;
	dynamicBufferInfo.range = m_DynamicAllignment;

	vk::DescriptorImageInfo image_info;
	image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	image_info.imageView = m_TextureImage->m_ImageView;
	image_info.sampler = m_TextureSampler;

	std::array<vk::WriteDescriptorSet, 3> descriptorWrites = {
		vk::WriteDescriptorSet(m_DescriptorSet, 0)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBuffer)
			.setPBufferInfo(&bufferInfo),
		vk::WriteDescriptorSet(m_DescriptorSet, 1)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eUniformBufferDynamic)
			.setPBufferInfo(&dynamicBufferInfo),
		vk::WriteDescriptorSet(m_DescriptorSet, 2)
			.setDescriptorCount(1)
			.setDescriptorType(vk::DescriptorType::eCombinedImageSampler)
			.setPImageInfo(&image_info),
	};

	m_Device->updateDescriptorSets(descriptorWrites, {});
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

	vk::ImageView image_view = m_Device->createImageView(view_info);

	return image_view;
}

void HelloTriangleApplication::createTextureSampler()
{
	vk::SamplerCreateInfo sampler_create_info = {};
	sampler_create_info.magFilter = vk::Filter::eLinear;
	sampler_create_info.minFilter = vk::Filter::eLinear;
	sampler_create_info.addressModeU = vk::SamplerAddressMode::eRepeat;
	sampler_create_info.addressModeV = vk::SamplerAddressMode::eRepeat;
	sampler_create_info.addressModeW = vk::SamplerAddressMode::eRepeat;
	sampler_create_info.anisotropyEnable = VK_TRUE;
	sampler_create_info.maxAnisotropy = 16;
	sampler_create_info.borderColor = vk::BorderColor::eIntOpaqueBlack;
	sampler_create_info.unnormalizedCoordinates = VK_FALSE;
	sampler_create_info.compareEnable = VK_FALSE;
	sampler_create_info.compareOp = vk::CompareOp::eAlways;
	sampler_create_info.mipmapMode = vk::SamplerMipmapMode::eLinear;
	sampler_create_info.mipLodBias = 0.0f;
	sampler_create_info.minLod = 0.0f;
	sampler_create_info.maxLod = 0.0f;

	m_TextureSampler = m_Device->createSampler(sampler_create_info);
}

vk::Format HelloTriangleApplication::findSupportedFormat(const vk::PhysicalDevice& device, const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features)
{
	for(auto& format : candidates)
	{
		auto properties = device.getFormatProperties(format);

		if ((tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features)  == features) || 
			(tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

vk::Format HelloTriangleApplication::findDepthFormat(const vk::PhysicalDevice& device)
{
	return findSupportedFormat(
		device,
		{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint},
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);
}

inline bool hasStencilComponent(vk::Format format)
{
	return format == vk::Format::eD32SfloatS8Uint|| format == vk::Format::eD24UnormS8Uint;
}

Image HelloTriangleApplication::createDepthResources(const vk::PhysicalDevice& physical_device, const vk::Device& logical_device, const Swapchain& swapchain)
{
	auto depth_format = findDepthFormat(physical_device);

	vk::ImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.extent.width = swapchain.width();
	imageCreateInfo.extent.height = swapchain.height();
	imageCreateInfo.extent.depth = 1;
	imageCreateInfo.mipLevels = 1;
	imageCreateInfo.arrayLayers = 1;
	imageCreateInfo.format = depth_format;
	imageCreateInfo.tiling = vk::ImageTiling::eOptimal;
	imageCreateInfo.usage = vk::ImageUsageFlagBits::eDepthStencilAttachment;
	imageCreateInfo.samples = vk::SampleCountFlagBits::e1;

	auto depth_image = Image(logical_device, physical_device, imageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageAspectFlagBits::eDepth);

	transitionImageLayout(depth_image.m_Image, depth_image.m_Format, vk::ImageLayout::eUndefined, vk::ImageLayout::eDepthStencilAttachmentOptimal);
	return depth_image;
}

void HelloTriangleApplication::createQueryPool()
{
	vk::QueryPoolCreateInfo query_pool_create_info;
	query_pool_create_info
		.setQueryType(vk::QueryType::ePipelineStatistics)
		.setQueryCount(m_SwapChain.framebuffers().size())
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
	m_QueryPool = m_Device->createQueryPool(query_pool_create_info);
}

// Initializes Vulkan
void HelloTriangleApplication::initVulkan() {
	m_RenderPass = createRenderPass(m_Device, m_SwapChain);
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createCommandPool(QueueFamilyIndices::findQueueFamilies(m_Device, m_Surface));
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
	vk::SemaphoreCreateInfo semaphoreInfo = {};

	m_ImageAvaliableSemaphore = m_Device->createSemaphore(semaphoreInfo);
	m_RenderFinishedSemaphore = m_Device->createSemaphore(semaphoreInfo);
}

 void HelloTriangleApplication::createCommandBuffers() {
	m_CommandBuffers.resize(m_SwapChain.framebuffers().size());

	//allocate room for buffers in command pool:

	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.commandPool = m_CommandPool;
	allocInfo.level = vk::CommandBufferLevel::ePrimary; //buffers can be primary (called to by user) or secondary (called to by primary buffer)
	allocInfo.commandBufferCount = m_CommandBuffers.size();

	m_CommandBuffers = m_Device->allocateCommandBuffers(allocInfo);

	//begin recording process:
	for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
		auto& command_buffer = m_CommandBuffers[i];

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		command_buffer.begin(beginInfo);

		//starting render pass:
		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = m_RenderPass;
		renderPassInfo.framebuffer = m_SwapChain.framebuffers()[i];

		renderPassInfo.renderArea.offset = vk::Offset2D{ 0,0 };
		renderPassInfo.renderArea.extent = m_SwapChain.extent();

		std::array<vk::ClearValue, 2> clear_values = {};
		clear_values[0].color = vk::ClearColorValue(std::array<float, 4>{0.2f, 0.3f, 0.8f, 1.0f});
		clear_values[1].depthStencil = vk::ClearDepthStencilValue{1.0f, 0};

		renderPassInfo.clearValueCount = clear_values.size();
		renderPassInfo.pClearValues = clear_values.data();

		command_buffer.beginRenderPass(renderPassInfo, vk::SubpassContents::eInline);

		command_buffer.bindPipeline(vk::PipelineBindPoint::eGraphics, m_GraphicsPipeline);

		command_buffer.bindVertexBuffers(
			0,								// index of first buffer
			{ m_VertexBuffer->m_Buffer },	// Array of buffers
			{ 0 });							// Array of offsets into the buffers
		command_buffer.bindIndexBuffer(m_IndexBuffer->m_Buffer, 0, vk::IndexType::eUint16);

		command_buffer.beginQuery(m_QueryPool, i, vk::QueryControlFlags());

		for (int j = 0; j < m_Scene.renderObjects().size(); j++) {
			uint32_t dynamic_offset = j * m_DynamicAllignment;
			command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0, { m_DescriptorSet }, { dynamic_offset });

			command_buffer.drawIndexed(s_Indices.size(), 1, 0, 0, 0);
		}

		command_buffer.endQuery(m_QueryPool, i);
		command_buffer.endRenderPass();
		command_buffer.end();
	}
}

 void HelloTriangleApplication::createCommandPool(const QueueFamilyIndices& indices) {
	vk::CommandPoolCreateInfo poolInfo = {};
	poolInfo.queueFamilyIndex = indices.graphicsFamily;
	poolInfo.flags = vk::CommandPoolCreateFlags(); //optional about rerecording strategy of cmd buffer(s)

	m_CommandPool = m_Device->createCommandPool(poolInfo);
}

 void HelloTriangleApplication::createFramebuffers() {
	m_SwapChain.framebuffers().resize(m_SwapChain.imageViews().size());

	for (size_t i = 0; i < m_SwapChain.imageViews().size(); i++) {
		std::array<vk::ImageView, 2> attachments = {
			m_SwapChain.imageViews()[i],
			m_DepthImage.m_ImageView
		};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = m_SwapChain.width();
		framebufferInfo.height = m_SwapChain.height();
		framebufferInfo.layers = 1;

		m_SwapChain.framebuffers()[i] = m_Device->createFramebuffer(framebufferInfo);


	}
}

 vk::RenderPass HelloTriangleApplication::createRenderPass(const vk::PhysicalDevice& physical_device, const vk::Device& device, const Swapchain& swapchain) {

	// Color buffer resides as swapchain image. 
	vk::AttachmentDescription colorAttatchment;
	colorAttatchment.setFormat(swapchain.imageFormat())
		.setLoadOp(vk::AttachmentLoadOp::eClear)
		.setStencilLoadOp(vk::AttachmentLoadOp::eLoad)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setFinalLayout(vk::ImageLayout::ePresentSrcKHR);

	vk::AttachmentReference colorAttatchmentRef(0, vk::ImageLayout::eColorAttachmentOptimal);


	// Depth buffer resides as swapchain image. 
	vk::AttachmentDescription depthAttatchment;
	depthAttatchment.setFormat(findDepthFormat(physical_device))
		.setLoadOp(vk::AttachmentLoadOp::eClear) // Clear buffer data at load
		.setStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setStencilLoadOp(vk::AttachmentLoadOp::eDontCare)
		.setStencilStoreOp(vk::AttachmentStoreOp::eDontCare)
		.setFinalLayout(vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::AttachmentReference depthAttatchmentRef(1, vk::ImageLayout::eDepthStencilAttachmentOptimal);

	vk::SubpassDescription subpass = {};
	subpass.setPipelineBindPoint(vk::PipelineBindPoint::eGraphics)
		.setColorAttachmentCount(1)
		.setPColorAttachments(&colorAttatchmentRef)
		.setPDepthStencilAttachment(&depthAttatchmentRef);

	// Handling subpass dependencies
	vk::SubpassDependency dependency(VK_SUBPASS_EXTERNAL);
	dependency.setSrcStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setSrcAccessMask(vk::AccessFlags())
		.setDstStageMask(vk::PipelineStageFlagBits::eColorAttachmentOutput)
		.setDstAccessMask(vk::AccessFlagBits::eColorAttachmentRead | vk::AccessFlagBits::eColorAttachmentWrite);

	std::array<vk::AttachmentDescription, 2> attachments = { colorAttatchment, depthAttatchment };
	
	vk::RenderPassCreateInfo renderPassInfo;
	renderPassInfo.setAttachmentCount(attachments.size())
		.setPAttachments(attachments.data())
		.setSubpassCount(1)
		.setPSubpasses(&subpass)
		.setDependencyCount(1)
		.setPDependencies(&dependency);

	return device.createRenderPass(renderPassInfo);
}

 void HelloTriangleApplication::createGraphicsPipeline() {

	//get byte code of shaders
	auto vertShader = Shader(*m_Device, "./shaders/vert.spv", vk::ShaderStageFlagBits::eVertex);
	auto fragShader = Shader(*m_Device, "./shaders/frag.spv", vk::ShaderStageFlagBits::eFragment);

	//for later reference:
	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShader.m_Info, fragShader.m_Info };

	auto bindingDescription = Vertex::getBindingDescription();
	auto attribute_descriptions = Vertex::getAttributeDescriptions();

	// Information on how to read from vertex buffer
	vk::PipelineVertexInputStateCreateInfo vertexInputInfo;
	vertexInputInfo.setVertexBindingDescriptionCount(1)
		.setPVertexBindingDescriptions(&bindingDescription)
		.setVertexAttributeDescriptionCount(attribute_descriptions.size())
		.setPVertexAttributeDescriptions(attribute_descriptions.data());

	vk::PipelineInputAssemblyStateCreateInfo inputAssembly;
	inputAssembly.setTopology(vk::PrimitiveTopology::eTriangleList);

	// Creating a viewport to render to
	vk::Viewport viewport(0.0f, 0.0f, m_SwapChain.width(), m_SwapChain.height(), 0.0f, 1.0f);

	// Scissor rectangle. Defines image cropping of viewport.
	vk::Rect2D scissor({ 0, 0 }, m_SwapChain.extent());

	// Combine viewport and scissor into a viewport state
	vk::PipelineViewportStateCreateInfo viewportState;
	viewportState.setViewportCount(1)
		.setPViewports(&viewport)
		.setScissorCount(1)
		.setPScissors(&scissor);

	vk::PipelineRasterizationStateCreateInfo rasterizer;
	rasterizer.setPolygonMode(vk::PolygonMode::eFill)
		.setLineWidth(1.0f)
		.setCullMode(vk::CullModeFlagBits::eBack)
		.setFrontFace(vk::FrontFace::eCounterClockwise);

	// Multisampling. Not in use but works to do anti aliasing
	vk::PipelineMultisampleStateCreateInfo multisampling = {};
	multisampling.setRasterizationSamples(vk::SampleCountFlagBits::e1)
		.setMinSampleShading(1.0f); //optional

	//Color blending
	vk::PipelineColorBlendAttachmentState colorBlendAttatchment;
	colorBlendAttatchment.setColorWriteMask(vk::ColorComponentFlagBits::eR | vk::ColorComponentFlagBits::eG | vk::ColorComponentFlagBits::eB | vk::ColorComponentFlagBits::eA)
		.setSrcColorBlendFactor(vk::BlendFactor::eOne)  // optional
		.setSrcAlphaBlendFactor(vk::BlendFactor::eOne); // optional

	//Global Color Blending
	vk::PipelineColorBlendStateCreateInfo colorBlending;
	colorBlending.setLogicOp(vk::LogicOp::eCopy)  // optional
		.setAttachmentCount(1)
		.setPAttachments(&colorBlendAttatchment);

	 // For uniforms
	vk::PipelineLayoutCreateInfo pipelineLayoutInfo;
	pipelineLayoutInfo.setSetLayoutCount(1)
		.setPSetLayouts(&m_DescriptorSetLayout);

	m_PipelineLayout = m_Device->createPipelineLayout(pipelineLayoutInfo);

	vk::PipelineDepthStencilStateCreateInfo depth_stencil_info;
	depth_stencil_info.setDepthTestEnable(true)
		.setDepthWriteEnable(true)
		.setDepthCompareOp(vk::CompareOp::eLess)
		.setMaxDepthBounds(1.0f);

	vk::GraphicsPipelineCreateInfo pipelineInfo;
	pipelineInfo.setStageCount(2)
		.setPStages(shaderStages)
		.setPVertexInputState(&vertexInputInfo)
		.setPInputAssemblyState(&inputAssembly)
		.setPViewportState(&viewportState)
		.setPRasterizationState(&rasterizer)
		.setPMultisampleState(&multisampling)
		.setPDepthStencilState(&depth_stencil_info)
		.setPColorBlendState(&colorBlending)
		.setLayout(m_PipelineLayout)
		.setRenderPass(m_RenderPass);

	m_GraphicsPipeline = m_Device->createGraphicsPipeline(vk::PipelineCache(), pipelineInfo);
}

vk::SurfaceKHR HelloTriangleApplication::createSurface(const Window& window, const Instance& instance)
{
	 VkWin32SurfaceCreateInfoKHR surface_info = {};
	 surface_info.sType = VK_STRUCTURE_TYPE_WIN32_SURFACE_CREATE_INFO_KHR;
	 surface_info.hwnd = window.GetHandle();
	 surface_info.hinstance = GetModuleHandle(nullptr);

	 auto CreateWin32SurfaceKHR = reinterpret_cast<PFN_vkCreateWin32SurfaceKHR>(instance->getProcAddr("vkCreateWin32SurfaceKHR"));

	 VkSurfaceKHR surface;

	 if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(static_cast<VkInstance>(*instance), &surface_info, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS) {
		 throw std::runtime_error("failed to create window surface!");
	 }

	 return vk::SurfaceKHR(surface);
}

vk::Device HelloTriangleApplication::createLogicalDevice(const vk::SurfaceKHR surface, const vk::PhysicalDevice& physical_device, const QueueFamilyIndices& indices) {
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = { indices.graphicsFamily, indices.presentFamily };
	float queuePriorty = 1.0f;

	// Runs over each family and makes a createinfo object for them
	for (int queueFamily : uniqueQueueFamilies) {
		vk::DeviceQueueCreateInfo queueCreateInfo = {};
		queueCreateInfo.queueFamilyIndex = queueFamily;
		queueCreateInfo.queueCount = 1;
		queueCreateInfo.pQueuePriorities = &queuePriorty; // Priority required even with 1 queue
		queueCreateInfos.push_back(queueCreateInfo);
	}


	vk::PhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.pipelineStatisticsQuery = VK_TRUE;

												  // Creating VkDeviceCreateInfo object
	vk::DeviceCreateInfo createInfo = {};
	createInfo.queueCreateInfoCount = queueCreateInfos.size();
	createInfo.pQueueCreateInfos = queueCreateInfos.data();
	createInfo.pEnabledFeatures = &deviceFeatures;
	createInfo.enabledExtensionCount = s_DeviceExtensions.size();
	createInfo.ppEnabledExtensionNames = s_DeviceExtensions.data();
	createInfo.enabledLayerCount = 0;

	auto logical_device = physical_device.createDevice(createInfo);

	// Get handle to queue in the logicalDevice
	m_GraphicsQueue = logical_device.getQueue(indices.graphicsFamily, 0);
	m_PresentQueue = logical_device.getQueue(indices.presentFamily, 0);

	return logical_device;
}

 vk::PhysicalDevice HelloTriangleApplication::pickPhysicalDevice(const Instance& instance, const vk::SurfaceKHR& surface) const
 {	
	for (auto& device : instance->enumeratePhysicalDevices()) {
		auto indices = QueueFamilyIndices::findQueueFamilies(device, surface);
		if(isDeviceSuitable(device, surface, indices)){
			return device;
		}
	}
	
	throw std::runtime_error("failed to find suitable GPU!");
}

 bool HelloTriangleApplication::isDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const QueueFamilyIndices& indices) const
 {
	bool extensionsSupported = checkDeviceExtensionSupport(device);
	bool swapChainAdequate = false;
	if (extensionsSupported) {
		auto swapChainSupport = querySwapChainSupport(device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentmodes.empty();
	}

	 auto supportedFeatures = device.getFeatures();

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
		VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
#ifndef NDEBUG
		VK_EXT_DEBUG_REPORT_EXTENSION_NAME
#endif
	};

	return extensions;
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

 vk::Extent2D HelloTriangleApplication::chooseSwapExtend(const vk::SurfaceCapabilitiesKHR & capabilities) const
 {
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max()) {
		return capabilities.currentExtent;
	}

	vk::Extent2D actualExtent = { m_Window.width(), m_Window.height() };
	
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
	m_Device->waitIdle();
}

void HelloTriangleApplication::drawFrame() {

	// Aquire image
	auto imageResult = m_Device->acquireNextImageKHR(*m_SwapChain, std::numeric_limits<uint64_t>::max(), m_ImageAvaliableSemaphore, vk::Fence());
	
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
	vk::Semaphore  waitSemaphores[] = { m_ImageAvaliableSemaphore };
	vk::PipelineStageFlags waitStages[] = {
		vk::PipelineStageFlagBits::eColorAttachmentOutput
	};
	submitInfo.waitSemaphoreCount = 1;
	submitInfo.pWaitSemaphores = waitSemaphores;
	submitInfo.pWaitDstStageMask = waitStages;

	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &m_CommandBuffers[imageResult.value]; // Use command buffers for aquired image

															  // Specify which sempahore to signal once command buffers have been executed.
	vk::Semaphore signalSemaphores[] = { m_RenderFinishedSemaphore };
	submitInfo.signalSemaphoreCount = 1;
	submitInfo.pSignalSemaphores = signalSemaphores;

	m_GraphicsQueue.submit({ submitInfo }, vk::Fence());

	// Present the image presented
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;

	vk::SwapchainKHR swapChains[] = { *m_SwapChain };
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageResult.value;

	presentInfo.pResults = nullptr; // Would contain VK result for all images if more than 1

	m_PresentQueue.presentKHR(presentInfo);
	m_PresentQueue.waitIdle();

	PipelineStatisticsResult queryResults;

	if(m_Device->getQueryPoolResults(
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

	_aligned_free(m_InstanceUniformBufferObject.model);

	m_Device->destroySampler(m_TextureSampler);
	m_Device->destroyDescriptorPool(m_DescriptorPool);
	m_Device->destroyDescriptorSetLayout(m_DescriptorSetLayout);

	m_Device->destroySemaphore(m_RenderFinishedSemaphore);
	m_Device->destroySemaphore(m_ImageAvaliableSemaphore);

	m_Device->destroyCommandPool(m_CommandPool);

	m_Device->destroyQueryPool(m_QueryPool);

	m_Instance->destroySurfaceKHR(m_Surface);
}

void HelloTriangleApplication::recreateSwapChain()
{
	m_Device->waitIdle();

	cleanupSwapChain();

	m_SwapChain = Swapchain(m_Window, m_Surface, m_Device);
	createRenderPass(m_Device, m_SwapChain);
	createGraphicsPipeline();
	createFramebuffers();
	createCommandBuffers();
}

void HelloTriangleApplication::cleanupSwapChain()
{
	m_Device->waitIdle();
	
	for (auto frameBuffer  : m_SwapChain.framebuffers()) {
		m_Device->destroyFramebuffer(frameBuffer);
	}

	m_Device->freeCommandBuffers(m_CommandPool, m_CommandBuffers);

	m_Device->destroyPipeline(m_GraphicsPipeline);

	m_Device->destroyPipelineLayout(m_PipelineLayout);

	m_Device->destroyRenderPass(m_RenderPass);
}

// TODO: Flyt denne metode ind i buffer klassen
void HelloTriangleApplication::copyBuffer(vk::Buffer source, vk::Buffer destination, vk::DeviceSize size)
{
	auto commandBuffer = beginSingleTimeCommands();

	vk::BufferCopy copyRegion;
	copyRegion.srcOffset = 0; // Optional
	copyRegion.dstOffset = 0; // Optional
	copyRegion.size = size;

	commandBuffer.copyBuffer(source, destination, { copyRegion });

	endSingleTimeCommands(commandBuffer);
}

void HelloTriangleApplication::createTextureImage()
{
	int texWidth, texHeight, texChannels;
	stbi_uc* pixels = stbi_load("textures/sample_image.jpg", &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);

	if (!pixels) {
		throw std::runtime_error("Failed to load texture image!");
	}
	vk::DeviceSize imageSize = texWidth * texHeight * 4;

	vk::BufferCreateInfo buffer_create_info;
	buffer_create_info.setSize(imageSize)
		.setUsage(vk::BufferUsageFlagBits::eTransferSrc);

	Buffer buffer(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible| vk::MemoryPropertyFlagBits::eHostCoherent);

	memcpy(buffer.map(), pixels, imageSize);
	buffer.unmap();

	stbi_image_free(pixels);

	vk::ImageCreateInfo imageCreateInfo;
	imageCreateInfo.setImageType(vk::ImageType::e2D)
		.setFormat(vk::Format::eR8G8B8A8Unorm)
		.setExtent({ static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight), 1 })
		.setMipLevels(1)
		.setArrayLayers(1)
		.setUsage(vk::ImageUsageFlagBits::eTransferDst | vk::ImageUsageFlagBits::eSampled)
		.setInitialLayout(vk::ImageLayout::eUndefined);

	m_TextureImage = std::make_unique<Image>(m_Device, imageCreateInfo, vk::MemoryPropertyFlagBits::eDeviceLocal, vk::ImageAspectFlagBits::eColor);
	transitionImageLayout(m_TextureImage->m_Image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eUndefined, vk::ImageLayout::eTransferDstOptimal);
	copyBufferToImage(buffer.m_Buffer, m_TextureImage->m_Image, texWidth, texHeight);

	//prepare to use image in shader:
	transitionImageLayout(m_TextureImage->m_Image, vk::Format::eR8G8B8A8Unorm, vk::ImageLayout::eTransferDstOptimal, vk::ImageLayout::eShaderReadOnlyOptimal);
}

vk::CommandBuffer HelloTriangleApplication::beginSingleTimeCommands() const
{
	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	vk::CommandBuffer commandBuffer = m_Device->allocateCommandBuffers(allocInfo)[0];

	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBuffer.begin(beginInfo);
	return commandBuffer;
}

void HelloTriangleApplication::endSingleTimeCommands(vk::CommandBuffer commandBuffer)
{
	commandBuffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &commandBuffer;

	m_GraphicsQueue.submit({ submitInfo }, vk::Fence());
	m_GraphicsQueue.waitIdle();

	m_Device->freeCommandBuffers(m_CommandPool, { m_CommandBuffers });
}

void HelloTriangleApplication::transitionImageLayout(vk::Image image, vk::Format format, vk::ImageLayout oldLayout, vk::ImageLayout newLayout)
{
	auto commandBuffer = beginSingleTimeCommands();

	vk::ImageAspectFlags aspect_mask = vk::ImageAspectFlagBits::eColor;

	// Special case for depth buffer image
	if(newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal)
	{
		aspect_mask = vk::ImageAspectFlagBits::eDepth;
		if(hasStencilComponent(format))
		{
			aspect_mask |= vk::ImageAspectFlagBits::eStencil;
		}
	}
	
	vk::PipelineStageFlags sourceStage;
	vk::PipelineStageFlags destinationStage;
	vk::AccessFlags source_access_mask;
	vk::AccessFlags destination_access_mask;

	if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eTransferDstOptimal) {
		source_access_mask = vk::AccessFlags();
		destination_access_mask = vk::AccessFlagBits::eTransferWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eTransfer;
	}
	else if (oldLayout == vk::ImageLayout::eTransferDstOptimal && newLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
		source_access_mask = vk::AccessFlagBits::eTransferWrite;
		destination_access_mask = vk::AccessFlagBits::eShaderRead;

		sourceStage = vk::PipelineStageFlagBits::eTransfer;
		destinationStage = vk::PipelineStageFlagBits::eFragmentShader;
	}
	else if (oldLayout == vk::ImageLayout::eUndefined && newLayout == vk::ImageLayout::eDepthStencilAttachmentOptimal) {
		source_access_mask = vk::AccessFlags();
		destination_access_mask = vk::AccessFlagBits::eDepthStencilAttachmentRead | vk::AccessFlagBits::eDepthStencilAttachmentWrite;

		sourceStage = vk::PipelineStageFlagBits::eTopOfPipe;
		destinationStage = vk::PipelineStageFlagBits::eEarlyFragmentTests;
	}
	else {
		throw std::invalid_argument("unsupported layout transition!");
	}

	auto image_memory_barrier = vk::ImageMemoryBarrier(
		source_access_mask, 
		destination_access_mask, 
		oldLayout, 
		newLayout, 
		VK_QUEUE_FAMILY_IGNORED, 
		VK_QUEUE_FAMILY_IGNORED, 
		image, 
		{ aspect_mask, 0, 1, 0, 1 });

	commandBuffer.pipelineBarrier(sourceStage, destinationStage, vk::DependencyFlags(), {}, {}, { image_memory_barrier });

	endSingleTimeCommands(commandBuffer);
}

void HelloTriangleApplication::copyBufferToImage(vk::Buffer buffer, vk::Image image, uint32_t width, uint32_t height)
{
	auto commandBuffer = beginSingleTimeCommands();

	vk::BufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = vk::Offset3D { 0,0,0 };
	region.imageExtent = vk::Extent3D {
		width,
		height,
		1
	};

	commandBuffer.copyBufferToImage(buffer, image, vk::ImageLayout::eTransferDstOptimal, { region });

	endSingleTimeCommands(commandBuffer);
}



