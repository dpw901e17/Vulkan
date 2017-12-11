#include "HelloTriangleApplication.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_RADIANS
#define STB_IMAGE_IMPLEMENTATION

#include <windows.h>	// For Beeps included early for not redifining max

#include <chrono>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <iostream>
#include <set>
#include <stb/stb_image.h>
#include <thread>		// For non blocking beeps
#include <vulkan/vulkan.h>

#include "QueueFamilyIndices.h"
#include "Vertex.h"
#include "Shader.h"
#include "Image.h"
#include "Utility.h"

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

vk::DeviceSize dynamicAllignment(const Device& device, size_t min_size)
{
	auto properties = device.getPhysicalProperties();
	auto allignment = properties.limits.minUniformBufferOffsetAlignment;

	if (allignment > 0)
	{
		return (min_size + allignment - 1) & ~(allignment - 1);
	}
	return min_size;
}

vk::DeviceSize HelloTriangleApplication::dynamicBufferSize(const Scene& scene, const Device& device)
{
	return scene.renderObjects().size() * dynamicAllignment(device, sizeof(glm::mat4));
}

HelloTriangleApplication::HelloTriangleApplication(Scene scene)
	: m_Scene(scene),
	  m_Window(GetModuleHandle(nullptr), "VulkanTest", "Vulkan Test", WIDTH, HEIGHT),
	  m_Instance(),
	  m_Surface(createSurface(m_Window, m_Instance)),
	  m_Device(m_Instance, m_Surface),
	  m_CommandPool(m_Device, QueueFamilyIndices::findQueueFamilies(static_cast<vk::PhysicalDevice>(m_Device), m_Surface).graphicsFamily),
	  m_SwapChain(m_Window, m_Surface, m_Device, m_CommandPool),
	  m_VertexBuffer(m_Device, vk::BufferCreateInfo().setSize(s_Vertices.size()).setUsage(vk::BufferUsageFlagBits::eVertexBuffer), vk::MemoryPropertyFlagBits::eDeviceLocal, (void*)s_Vertices.data(), m_CommandPool),
	  m_IndexBuffer(m_Device, vk::BufferCreateInfo().setSize(s_Indices.size()).setUsage(vk::BufferUsageFlagBits::eIndexBuffer), vk::MemoryPropertyFlagBits::eDeviceLocal, (void*)s_Indices.data(), m_CommandPool),
	  m_UniformBuffer(m_Device, vk::BufferCreateInfo().setSize(sizeof(m_UniformBufferObject)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer), vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent),
	  m_DynamicUniformBuffer(m_Device, vk::BufferCreateInfo().setSize(dynamicBufferSize(m_Scene, m_Device)).setUsage(vk::BufferUsageFlagBits::eUniformBuffer), vk::MemoryPropertyFlagBits::eHostVisible), 
	  m_TextureImage("textures/sample_image.jpg", m_Device, m_CommandPool)
{
	m_InstanceUniformBufferObject.model = static_cast<glm::mat4 *>(_aligned_malloc(dynamicBufferSize(m_Scene, m_Device), dynamicAllignment(m_Device, sizeof(glm::mat4))));;
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
	bufferInfo.buffer = static_cast<vk::Buffer>(m_UniformBuffer);
	bufferInfo.offset = 0;
	bufferInfo.range = m_UniformBuffer.size();

	vk::DescriptorBufferInfo dynamicBufferInfo;
	dynamicBufferInfo.buffer = static_cast<vk::Buffer>(m_DynamicUniformBuffer);
	dynamicBufferInfo.offset = 0;
	dynamicBufferInfo.range = dynamicAllignment(m_Device, sizeof(glm::mat4));;

	vk::DescriptorImageInfo image_info;
	image_info.imageLayout = vk::ImageLayout::eShaderReadOnlyOptimal;
	image_info.imageView = static_cast<vk::ImageView>(m_TextureImage);
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
	createDescriptorSetLayout();
	createGraphicsPipeline();
	createTextureSampler();
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
	allocInfo.commandPool = static_cast<vk::CommandPool>(m_CommandPool);
	allocInfo.level = vk::CommandBufferLevel::ePrimary; //buffers can be primary (called to by user) or secondary (called to by primary buffer)
	allocInfo.commandBufferCount = m_CommandBuffers.size();

	m_CommandBuffers = m_Device->allocateCommandBuffers(allocInfo);

	auto allignment = dynamicAllignment(m_Device, sizeof(glm::mat4));

	//begin recording process:
	for (size_t i = 0; i < m_CommandBuffers.size(); i++) {
		auto& command_buffer = m_CommandBuffers[i];

		vk::CommandBufferBeginInfo beginInfo = {};
		beginInfo.flags = vk::CommandBufferUsageFlagBits::eSimultaneousUse;

		command_buffer.begin(beginInfo);

		//starting render pass:
		vk::RenderPassBeginInfo renderPassInfo = {};
		renderPassInfo.renderPass = m_SwapChain.renderPass();
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
			{ static_cast<vk::Buffer>(m_VertexBuffer)},	// Array of buffers
			{ 0 });							// Array of offsets into the buffers
		command_buffer.bindIndexBuffer(static_cast<vk::Buffer>(m_IndexBuffer), 0, vk::IndexType::eUint16);

		command_buffer.beginQuery(m_QueryPool, i, vk::QueryControlFlags());

		for (int j = 0; j < m_Scene.renderObjects().size(); j++) {
			uint32_t dynamic_offset = j * allignment;
			command_buffer.bindDescriptorSets(vk::PipelineBindPoint::eGraphics, m_PipelineLayout, 0, { m_DescriptorSet }, { dynamic_offset });

			command_buffer.drawIndexed(s_Indices.size(), 1, 0, 0, 0);
		}

		command_buffer.endQuery(m_QueryPool, i);
		command_buffer.endRenderPass();
		command_buffer.end();
	}
}

 void HelloTriangleApplication::createGraphicsPipeline() {

	//get byte code of shaders
	Shader vertShader(static_cast<vk::Device>(m_Device), "./shaders/vert.spv", vk::ShaderStageFlagBits::eVertex);
	Shader fragShader(static_cast<vk::Device>(m_Device), "./shaders/frag.spv", vk::ShaderStageFlagBits::eFragment);

	//for later reference:
	vk::PipelineShaderStageCreateInfo shaderStages[] = { vertShader.info(), fragShader.info() };

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
		.setRenderPass(m_SwapChain.renderPass());

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

	 if (!CreateWin32SurfaceKHR || CreateWin32SurfaceKHR(static_cast<VkInstance>(instance), &surface_info, nullptr, reinterpret_cast<VkSurfaceKHR*>(&surface)) != VK_SUCCESS) {
		 throw std::runtime_error("failed to create window surface!");
	 }

	 return vk::SurfaceKHR(surface);
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

	memcpy(m_UniformBuffer.map(), &m_UniformBufferObject, sizeof(m_UniformBufferObject));
	m_UniformBuffer.unmap();
}

void HelloTriangleApplication::updateDynamicUniformBuffer() const
{
	auto allignment = dynamicAllignment(m_Device, sizeof(glm::mat4));
	for (auto index = 0; index < m_Scene.renderObjects().size(); index++)
	{
		auto& render_object = m_Scene.renderObjects()[index];
		auto model = reinterpret_cast<glm::mat4*>(reinterpret_cast<uint64_t>(m_InstanceUniformBufferObject.model) + (index * allignment));
		*model = translate(glm::mat4(), { render_object.x(), render_object.y(), render_object.z() });
	}

	memcpy(m_DynamicUniformBuffer.map(), m_InstanceUniformBufferObject.model, allignment * m_Scene.renderObjects().size());
	m_DynamicUniformBuffer.unmap();
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

	m_Device.submitToGraphicsQueue(submitInfo);

	vk::SwapchainKHR swapChains[] = { *m_SwapChain };
	// Present the image presented
	vk::PresentInfoKHR presentInfo = {};
	presentInfo.waitSemaphoreCount = 1;
	presentInfo.pWaitSemaphores = signalSemaphores;
	presentInfo.swapchainCount = 1;
	presentInfo.pSwapchains = swapChains;
	presentInfo.pImageIndices = &imageResult.value;

	presentInfo.pResults = nullptr; // Would contain VK result for all images if more than 1

	m_Device.present(presentInfo);

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

void HelloTriangleApplication::cleanup() const
{
	cleanupSwapChain();

	_aligned_free(m_InstanceUniformBufferObject.model);

	m_Device->destroySampler(m_TextureSampler);
	m_Device->destroyDescriptorPool(m_DescriptorPool);
	m_Device->destroyDescriptorSetLayout(m_DescriptorSetLayout);

	m_Device->destroySemaphore(m_RenderFinishedSemaphore);
	m_Device->destroySemaphore(m_ImageAvaliableSemaphore);

	m_Device->destroyQueryPool(m_QueryPool);

	m_Instance->destroySurfaceKHR(m_Surface);
}

void HelloTriangleApplication::cleanupSwapChain() const
{
	m_Device->waitIdle();
	
	for (auto frameBuffer  : m_SwapChain.framebuffers()) {
		m_Device->destroyFramebuffer(frameBuffer);
	}

	m_Device->destroyPipeline(m_GraphicsPipeline);

	m_Device->destroyPipelineLayout(m_PipelineLayout);
}
