#pragma once
#include <vulkan/vulkan.hpp>

#include "Device.h"
#include "Image.h"
class CommandPool;

class Buffer
{
public:
	Buffer(const Device&, const vk::BufferCreateInfo&, const vk::MemoryPropertyFlags&);
	Buffer(const Device&, vk::BufferCreateInfo, const vk::MemoryPropertyFlags&, void* data, const CommandPool& command_pool);
	Buffer(const Buffer&) = delete;
	~Buffer();

	Buffer& operator =(const Buffer&) = delete;
	explicit operator vk::Buffer() const { return m_Buffer; }

	void copyTo(const Buffer& destination, const CommandPool& command_pool) const;
	void copyToImage(const Image&, uint32_t width, uint32_t height, const CommandPool&) const;


	void* map() const;
	void unmap() const;
	vk::DeviceSize size() const { return m_Size; }

private:
	vk::Buffer m_Buffer;
	vk::DeviceMemory m_Memory;
	const Device& m_Device;
	vk::DeviceSize m_Size;
};
