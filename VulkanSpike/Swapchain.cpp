#include "Swapchain.h"
#include "QueueFamilyIndices.h"
#include "Image.h"

Swapchain::Swapchain(const Window& window, const vk::SurfaceKHR& surface, const vk::PhysicalDevice& physical_device, const vk::Device& logical_device)
	: m_Device(logical_device)
{
	auto swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(surface, physical_device);

	auto surfaceFormat = chooseSwapSurfaceFormat(swapChainSupport.formats);
	auto presentMode = chooseSwapPresentMode(swapChainSupport.presentmodes);
	m_Extent = chooseSwapExtend(window, swapChainSupport.capabilities);

	auto imageCount = swapChainSupport.capabilities.minImageCount + 1;
	if (swapChainSupport.capabilities.maxImageCount > 0 &&
		imageCount > swapChainSupport.capabilities.maxImageCount)
	{
		imageCount = swapChainSupport.capabilities.maxImageCount;
	}

	vk::SwapchainCreateInfoKHR createInfo;
	createInfo.setSurface(surface)
		.setMinImageCount(imageCount)
		.setImageFormat(surfaceFormat.format)
		.setImageColorSpace(surfaceFormat.colorSpace)
		.setImageExtent(m_Extent)
		.setImageArrayLayers(1)
		.setImageUsage(vk::ImageUsageFlagBits::eColorAttachment)
		.setPreTransform(swapChainSupport.capabilities.currentTransform)
		.setCompositeAlpha(vk::CompositeAlphaFlagBitsKHR::eOpaque)
		.setPresentMode(presentMode)
		.setClipped(true);
	
	auto indices = QueueFamilyIndices::findQueueFamilies(physical_device, surface);

	if (indices.graphicsFamily != indices.presentFamily)
	{
		uint32_t queueFamilyIndices[] = {indices.graphicsFamily, indices.presentFamily};

		createInfo.setImageSharingMode(vk::SharingMode::eConcurrent)
			.setQueueFamilyIndexCount(2)
			.setPQueueFamilyIndices(queueFamilyIndices);
	}
	else
	{
		createInfo.setImageSharingMode(vk::SharingMode::eExclusive);
	}

	m_SwapChain = logical_device.createSwapchainKHR(createInfo);
	m_Images = logical_device.getSwapchainImagesKHR(m_SwapChain);
	m_ImageFormat = surfaceFormat.format;

	// Create Image views
	m_ImageViews.resize(m_Images.size());

	for (auto i = 0; i < m_Images.size(); i++) {
		vk::ImageViewCreateInfo view_info = {};
		view_info.image = m_Images[i];
		view_info.viewType = vk::ImageViewType::e2D;
		view_info.format = m_ImageFormat;
		view_info.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
		view_info.subresourceRange.baseMipLevel = 0;
		view_info.subresourceRange.levelCount = 1;
		view_info.subresourceRange.baseArrayLayer = 0;
		view_info.subresourceRange.layerCount = 1;

		m_ImageViews[i] = logical_device.createImageView(view_info);
	}

	// Create depth image
	auto depth_format = findDepthFormat(physical_device);

	vk::ImageCreateInfo imageCreateInfo = {};
	imageCreateInfo.imageType = vk::ImageType::e2D;
	imageCreateInfo.extent.width = width();
	imageCreateInfo.extent.height = height();
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

	// Create framebuffers
	m_Framebuffers.resize(m_ImageViews.size());
	for (size_t i = 0; i < m_ImageViews.size(); i++) {
		std::array<vk::ImageView, 2> attachments = {
			m_ImageViews[i],
			depth_image_view
		};

		vk::FramebufferCreateInfo framebufferInfo = {};
		framebufferInfo.renderPass = m_RenderPass;
		framebufferInfo.attachmentCount = attachments.size();
		framebufferInfo.pAttachments = attachments.data();
		framebufferInfo.width = width();
		framebufferInfo.height = height();
		framebufferInfo.layers = 1;

		m_Framebuffers[i] = m_Device.createFramebuffer(framebufferInfo);
	}
}

Swapchain::~Swapchain()
{
	for (auto imageView : m_ImageViews) {
		m_Device.destroyImageView(imageView);
	}
	m_Device.destroySwapchainKHR(m_SwapChain);
}

vk::SurfaceFormatKHR Swapchain::chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats)
{
	//In the case the surface has no preference
	if (availableFormats.size() == 1 && availableFormats[0].format == vk::Format::eUndefined)
	{
		return {vk::Format::eB8G8R8A8Unorm, vk::ColorSpaceKHR::eSrgbNonlinear};
	}

	// If we're not allowed to freely choose a format  
	for (const auto& availableFormat : availableFormats)
	{
		if (availableFormat.format == vk::Format::eB8G8R8A8Unorm &&
			availableFormat.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear)
		{
			return availableFormat;
		}
	}

	return availableFormats[0];
}

vk::PresentModeKHR Swapchain::chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes)
{
	auto bestMode = vk::PresentModeKHR::eFifo;

	for (const auto& availablePresentMode : availablePresentModes)
	{
		if (availablePresentMode == vk::PresentModeKHR::eMailbox)
		{
			return availablePresentMode;
		}
		if (availablePresentMode == vk::PresentModeKHR::eImmediate)
		{
			bestMode = availablePresentMode;
		}
	}

	return bestMode;
}

vk::Extent2D Swapchain::chooseSwapExtend(const Window& window, const vk::SurfaceCapabilitiesKHR& capabilities)
{
	if (capabilities.currentExtent.width != std::numeric_limits<uint32_t>::max())
	{
		return capabilities.currentExtent;
	}

	vk::Extent2D actualExtent = {window.width(), window.height()};

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
