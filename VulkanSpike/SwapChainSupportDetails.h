#pragma once
#include <vector>
#include <vulkan/vulkan.hpp>

struct SwapChainSupportDetails {
	vk::SurfaceCapabilitiesKHR capabilities;
	std::vector<vk::SurfaceFormatKHR> formats;
	std::vector<vk::PresentModeKHR> presentmodes;

	static SwapChainSupportDetails querySwapChainSupport(const vk::SurfaceKHR& surface, const vk::PhysicalDevice& device)
	{
		SwapChainSupportDetails details;
		details.capabilities = device.getSurfaceCapabilitiesKHR(surface);
		details.formats = device.getSurfaceFormatsKHR(surface);
		details.presentmodes = device.getSurfacePresentModesKHR(surface);

		return details;
	}
};