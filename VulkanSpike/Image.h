#pragma once
#include <vulkan/vulkan.hpp>
#include "Utility.h"
#include "CommandPool.h"

class Image
{
public:
	Image(const Device&, const vk::ImageCreateInfo&, const vk::MemoryPropertyFlags&, const vk::ImageAspectFlagBits&);
	Image(const Image&) = delete;
	~Image();

	void changeLayout(const vk::ImageLayout& new_layout, const CommandPool&);

	explicit operator vk::Image() const
	{
		return m_Image;
	}
private:
	vk::Image m_Image;
	vk::DeviceMemory m_Memory;
	vk::ImageView m_ImageView;
	vk::Format m_Format;
	vk::ImageLayout m_ImageLayout = vk::ImageLayout::eUndefined;
	Device m_Device;
};
