#pragma once
#include <vulkan/vulkan.hpp>

#define NOMINMAX // for excluding the min/max macro in windows.h from the window system

#include "SwapChainSupportDetails.h"
#include "../scene-window-system/Window.h"
#include "Device.h"
#include "Image.h"

class CommandPool;

class Swapchain
{
public:
	Swapchain(const Window& window, const vk::SurfaceKHR& surface, const Device& device, const CommandPool&);
	~Swapchain();

	uint32_t width() const { return m_Extent.width; }
	uint32_t height() const { return m_Extent.height; }
	vk::Extent2D extent() const { return m_Extent; }
	vk::Format imageFormat() const { return m_ImageFormat; }
	std::vector<vk::Image> images() const { return m_Images; }
	vk::SwapchainKHR operator*() const { return m_SwapChain; }
	std::vector<vk::Framebuffer> framebuffers() const { return m_Framebuffers; }
	std::vector<vk::ImageView> imageViews() const { return m_ImageViews; }
	vk::RenderPass renderPass() const { return m_RenderPass; }
private:
	static vk::SurfaceFormatKHR chooseSwapSurfaceFormat(const std::vector<vk::SurfaceFormatKHR>& availableFormats);
	static vk::PresentModeKHR chooseSwapPresentMode(const std::vector<vk::PresentModeKHR>& availablePresentModes);
	static vk::Extent2D chooseSwapExtend(const Window& window, const vk::SurfaceCapabilitiesKHR& capabilities);
	static std::vector<vk::ImageView> createImageViews(const Device&, const std::vector<vk::Image>&, const vk::Format&);
	vk::SwapchainKHR createSwapchain(const vk::SurfaceKHR&, const SwapChainSupportDetails&, const vk::SurfaceFormatKHR&, const vk::PresentModeKHR&);
	static vk::RenderPass createRenderPass(const Device& device, const vk::Format&);
	std::vector<vk::Framebuffer> createFramebuffers(const Image& depth_image);

	const Device& m_Device;
	vk::Extent2D m_Extent;
	vk::SwapchainKHR m_SwapChain;
	std::vector<vk::Image> m_Images;
	vk::Format m_ImageFormat;
	std::vector<vk::ImageView> m_ImageViews;
	std::vector<vk::Framebuffer> m_Framebuffers;
	vk::RenderPass m_RenderPass;
};
