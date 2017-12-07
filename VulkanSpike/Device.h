#pragma once
#include "QueueFamilyIndices.h"
#include "Instance.h"

class Device
{
public:
	Device(const Instance& instance, const vk::SurfaceKHR surface);
	Device(const Device&) = delete;
	~Device();

	Device& operator =(const Device&) = delete;

	const vk::Device* operator ->() const { return &m_LogicalDevice; }
	explicit operator vk::Device() const { return m_LogicalDevice; }
	explicit operator vk::PhysicalDevice() const { return m_PhysicalDevice; }
	vk::PhysicalDeviceProperties getPhysicalProperties() const { return m_PhysicalDevice.getProperties(); }
	vk::Format findDepthFormat() const;
	vk::Format findSupportedFormat(const std::vector<vk::Format>& candidates, vk::ImageTiling, vk::FormatFeatureFlags) const;
	void submitToGraphicsQueue(const vk::SubmitInfo& submit_info) const
	{
		m_GraphicsQueue.submit({ submit_info }, vk::Fence());
	}
	void waitForGraphicsQueue() const
	{
		m_GraphicsQueue.waitIdle();
	}

	void present(vk::PresentInfoKHR) const;
private:
	static const std::vector<const char*> s_DeviceExtensions;

	static bool checkDeviceExtensionSupport(const vk::PhysicalDevice& device);
	static bool isDeviceSuitable(const vk::PhysicalDevice& device, const vk::SurfaceKHR& surface, const QueueFamilyIndices& indices);
	static vk::PhysicalDevice pickPhysicalDevice(const Instance& instance, const vk::SurfaceKHR surface);
	static vk::Device createLogicalDevice(const vk::SurfaceKHR surface, const vk::PhysicalDevice& physical_device);

	vk::Queue m_GraphicsQueue;
	vk::Queue m_PresentQueue;
	vk::PhysicalDevice m_PhysicalDevice;
	vk::Device m_LogicalDevice;
};
