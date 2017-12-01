#pragma once
#include <vulkan/vulkan.hpp>

class Buffer
{
public:
	//Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkDeviceSize size, VkBufferUsageFlags usage, VkMemoryPropertyFlags properties);
	Buffer(VkPhysicalDevice physicalDevice, VkDevice device, VkBufferCreateInfo buffer_info, VkMemoryPropertyFlags properties);
	~Buffer();

	VkResult map();
	void unmap();

	void* mappedMemory() const
	{
		if (m_Mapped == nullptr) throw std::runtime_error("Trying to get mapped memory of a buffer that is not mapped.");
		return m_Mapped ;
	}
	VkDeviceSize size() const { return m_Size; }

	VkBuffer m_Buffer;
	VkDeviceMemory m_Memory;
private:
	void* m_Mapped = nullptr;
	VkDevice m_Device;
	VkDeviceSize m_Size;
};