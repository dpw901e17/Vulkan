#include "CommandPool.h"

CommandPool::CommandPool(const Device & device, int graphics_family_index)
	: m_Device(device),
	m_CommandPool(device->createCommandPool(
		vk::CommandPoolCreateInfo().setQueueFamilyIndex(graphics_family_index)))
{

}

vk::CommandBuffer CommandPool::beginSingleTimeCommands() const
{
	vk::CommandBufferAllocateInfo allocInfo = {};
	allocInfo.level = vk::CommandBufferLevel::ePrimary;
	allocInfo.commandPool = m_CommandPool;
	allocInfo.commandBufferCount = 1;

	auto commandBuffer = m_Device->allocateCommandBuffers(allocInfo)[0];

	vk::CommandBufferBeginInfo beginInfo = {};
	beginInfo.flags = vk::CommandBufferUsageFlagBits::eOneTimeSubmit;
	commandBuffer.begin(beginInfo);
	return commandBuffer;
}

void CommandPool::endSingleTimeCommands(const vk::CommandBuffer& command_buffer) const
{
	command_buffer.end();

	vk::SubmitInfo submitInfo;
	submitInfo.commandBufferCount = 1;
	submitInfo.pCommandBuffers = &command_buffer;

	m_Device.submitToGraphicsQueue(submitInfo);
	m_Device.waitForGraphicsQueue();

	m_Device->freeCommandBuffers(m_CommandPool, { command_buffer });
}
