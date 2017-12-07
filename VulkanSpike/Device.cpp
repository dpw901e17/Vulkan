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

vk::Format Device::findDepthFormat() const
{
	return findSupportedFormat(
	{ vk::Format::eD32Sfloat, vk::Format::eD32SfloatS8Uint, vk::Format::eD24UnormS8Uint },
		vk::ImageTiling::eOptimal,
		vk::FormatFeatureFlagBits::eDepthStencilAttachment
	);
}

vk::Format Device::findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling tiling, vk::FormatFeatureFlags features) const
{
	for (auto& format : candidates)
	{
		auto properties = m_PhysicalDevice.getFormatProperties(format);

		if ((tiling == vk::ImageTiling::eLinear && (properties.linearTilingFeatures & features) == features) ||
			(tiling == vk::ImageTiling::eOptimal && (properties.optimalTilingFeatures & features) == features)) {
			return format;
		}
	}
	throw std::runtime_error("failed to find supported format!");
}

void Device::present(vk::PresentInfoKHR present_info) const
{
	m_PresentQueue.presentKHR(present_info);
	m_PresentQueue.waitIdle();
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

std::vector<vk::DeviceQueueCreateInfo> Device::findUniqueQueueFamilies(QueueFamilyIndices indices)
{
	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos;
	std::set<int> uniqueQueueFamilies = {indices.graphicsFamily, indices.presentFamily};
	auto queuePriorty = 1.0f;

	// Runs over each family and makes a createinfo object for them
	for (auto queueFamily : uniqueQueueFamilies)
	{
		queueCreateInfos.push_back(
			vk::DeviceQueueCreateInfo()
			.setQueueFamilyIndex(queueFamily)
			.setQueueCount(1)
			.setPQueuePriorities(&queuePriorty));
	}

	return queueCreateInfos;
}

vk::Device Device::createLogicalDevice(const vk::SurfaceKHR surface, const vk::PhysicalDevice& physical_device)
{
	auto indices = QueueFamilyIndices::findQueueFamilies(physical_device, surface);

	std::vector<vk::DeviceQueueCreateInfo> queueCreateInfos = findUniqueQueueFamilies(indices);

	vk::PhysicalDeviceFeatures deviceFeatures = {};
	deviceFeatures.samplerAnisotropy = VK_TRUE;
	deviceFeatures.pipelineStatisticsQuery = VK_TRUE;

	return physical_device.createDevice(
		vk::DeviceCreateInfo()
			.setQueueCreateInfoCount(queueCreateInfos.size())
			.setPQueueCreateInfos(queueCreateInfos.data())
			.setPEnabledFeatures(&deviceFeatures)
			.setEnabledExtensionCount(s_DeviceExtensions.size())
			.setPpEnabledExtensionNames(s_DeviceExtensions.data()));
}
