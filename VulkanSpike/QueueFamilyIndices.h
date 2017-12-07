#pragma once
#include <vulkan/vulkan.hpp>

struct QueueFamilyIndices {
	int graphicsFamily = -1; //<-- "not found"
	int presentFamily = -1;

	bool isComplete() const
	{
		return graphicsFamily >= 0 && presentFamily >= 0;
	}

	static QueueFamilyIndices findQueueFamilies(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface)
	{
		QueueFamilyIndices indices;

		auto queueFamilies = device.getQueueFamilyProperties();

		for (int i = 0; i < queueFamilies.size(); i++) {
			auto queueFamily = queueFamilies.at(i);

			//check for graphics family
			if (queueFamily.queueCount > 0 && queueFamily.queueFlags & vk::QueueFlagBits::eGraphics) {
				indices.graphicsFamily = i;
			}

			//check for present family
			if (queueFamily.queueCount > 0 && device.getSurfaceSupportKHR(i, surface)) {
				indices.presentFamily = i;
			}

			if (indices.isComplete()) {
				return indices;
			}
		}

		return indices;
	}
};
