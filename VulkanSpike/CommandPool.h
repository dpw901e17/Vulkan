#pragma once
#include <vulkan/vulkan.hpp>
#include "Device.h"

class CommandPool {
public:
	CommandPool(const Device& device, int graphics_family_index);
	CommandPool(const CommandPool&) = delete;
	CommandPool& operator =(const CommandPool&) = delete;


	vk::CommandBuffer beginSingleTimeCommands() const;

	void endSingleTimeCommands(const vk::CommandBuffer& command_buffer) const;

	explicit operator vk::CommandPool()
	{
		return m_CommandPool;
	}
private:
	Device m_Device;
	vk::CommandPool m_CommandPool;
};