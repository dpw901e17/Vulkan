#include "Device.h"
#include <set>
#include "SwapChainSupportDetails.h"

const std::vector<const char*> Device::s_DeviceExtensions = {
	VK_KHR_SWAPCHAIN_EXTENSION_NAME
};

Device::Device(const Instance& instance, const vk::SurfaceKHR surface): m_PhysicalDevice(pickPhysicalDevice(instance, surface)),
                                                                        m_LogicalDevice(createLogicalDevice(surface, m_PhysicalDevice))
{
	auto indices = QueueFamilyIndices::findQueueFamilies(m_PhysicalDevice, surface);

	m_GraphicsQueue = m_LogicalDevice.getQueue(indices.graphicsFamily, 0);
	m_PresentQueue = m_LogicalDevice.getQueue(indices.presentFamily, 0);
}

Device::~Device()
{
}

bool Device::checkDeviceExtensionSupport(const vk::PhysicalDevice& device)
{
	auto avaliableExtensions = device.enumerateDeviceExtensionProperties();
	std::set<std::string> requiredExtensions(s_DeviceExtensions.begin(), s_DeviceExtensions.end());

	for (const auto& extension : avaliableExtensions)
	{
		requiredExtensions.erase(extension.extensionName);
	}

	return requiredExtensions.empty();
}

bool Device::isDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const QueueFamilyIndices& indices)
{
	auto extensionsSupported = checkDeviceExtensionSupport(device);
	auto swapChainAdequate = false;
	if (extensionsSupported)
	{
		auto swapChainSupport = SwapChainSupportDetails::querySwapChainSupport(surface, device);
		swapChainAdequate = !swapChainSupport.formats.empty() && !swapChainSupport.presentmodes.empty();
	}

	auto supportedFeatures = device.getFeatures();

	return indices.isComplete() && extensionsSupported && swapChainAdequate && supportedFeatures.samplerAnisotropy;
}

vk::PhysicalDevice Device::pickPhysicalDevice(const Instance& instance, const vk::SurfaceKHR surface)
{
	for (auto& device : instance->enumeratePhysicalDevices())
	{
		auto indices = QueueFamilyIndices::findQueueFamilies(device, surface);
		if (isDeviceSuitable(device, surface, indices))
		{
			return device;
		}
	}
	throw std::runtime_error("failed to find suitable GPU.");
}

vk::Device Device::createLogicalDevice(const vk::SurfaceKHR surface, const vk::PhysicalDevice& physical_device)
{
	auto indices = QueueFamilyIndices::findQueueFamilies(physical_device, surface);

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
	float queuePriorty = 1.0f;

	// Runs over each family and makes a createinfo object for them
	for (int queueFamily : uniqueQueueFamilies)
	{
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

	return physical_device.createDevice(createInfo);
}
