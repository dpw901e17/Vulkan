#pragma once
#include "QueueFamilyIndices.h"
#include "Instance.h"

class Device
{
public:
	Device(const Instance& instance, const vk::SurfaceKHR surface);
	~Device();

	const vk::Device* operator ->() const { return &m_LogicalDevice; }
	const vk::Device& operator*() const { return m_LogicalDevice; }
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
