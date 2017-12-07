#include "Buffer.h"

#include "CommandPool.h"
#include "Utility.h"

Buffer::Buffer(const Device& device, const vk::BufferCreateInfo& buffer_info, const vk::MemoryPropertyFlags& properties)
	: m_Buffer(device->createBuffer(buffer_info)), m_Device(device), m_Size(buffer_info.size)
{
	auto memRequirements = device->getBufferMemoryRequirements(m_Buffer);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo
		.setAllocationSize(memRequirements.size)
		.setMemoryTypeIndex(findMemoryType(static_cast<vk::PhysicalDevice>(device), memRequirements.memoryTypeBits, properties));

	m_Memory = device->allocateMemory(allocInfo);

	device->bindBufferMemory(m_Buffer, m_Memory, 0);
}

void Buffer::copyTo(const Buffer& destination, const CommandPool& command_pool) const
{
	auto commandBuffer = command_pool.beginSingleTimeCommands();

	commandBuffer.copyBuffer(m_Buffer, destination.m_Buffer, vk::BufferCopy(0,0, std::min(m_Size, destination.m_Size)));

	command_pool.endSingleTimeCommands(commandBuffer);
}

void Buffer::copyToImage(const Image& image, uint32_t width, uint32_t height, const CommandPool& command_pool) const
{
	auto commandBuffer = command_pool.beginSingleTimeCommands();

	vk::BufferImageCopy region;
	region.bufferOffset = 0;
	region.bufferRowLength = 0;
	region.bufferImageHeight = 0;
	region.imageSubresource.aspectMask = vk::ImageAspectFlagBits::eColor;
	region.imageSubresource.mipLevel = 0;
	region.imageSubresource.baseArrayLayer = 0;
	region.imageSubresource.layerCount = 1;
	region.imageOffset = vk::Offset3D{0,0,0};
	region.imageExtent = vk::Extent3D{
		width,
		height,
		1
	};

	commandBuffer.copyBufferToImage(m_Buffer, static_cast<vk::Image>(image), vk::ImageLayout::eTransferDstOptimal, {region});

	command_pool.endSingleTimeCommands(commandBuffer);
}

Buffer::Buffer(const Device& device, vk::BufferCreateInfo buffer_info, const vk::MemoryPropertyFlags& properties, void* data, const CommandPool& command_pool)
	: m_Device(device), m_Size(buffer_info.size)
{
	// Make a temporary buffer
	vk::BufferCreateInfo buffer_create_info = {};
	buffer_create_info.size = buffer_info.size;
	buffer_create_info.usage = vk::BufferUsageFlagBits::eTransferSrc;

	Buffer buffer(m_Device, buffer_create_info, vk::MemoryPropertyFlagBits::eHostVisible | vk::MemoryPropertyFlagBits::eHostCoherent);

	// Copy data into the temporary buffer
	memcpy(buffer.map(), data, buffer_info.size);
	buffer.unmap();

	// Create the actual buffer
	buffer_info.usage |= vk::BufferUsageFlagBits::eTransferDst;
	m_Buffer = device->createBuffer(buffer_info);
	auto memRequirements = device->getBufferMemoryRequirements(m_Buffer);

	vk::MemoryAllocateInfo allocInfo = {};
	allocInfo
		.setAllocationSize(memRequirements.size)
		.setMemoryTypeIndex(findMemoryType(static_cast<vk::PhysicalDevice>(device), memRequirements.memoryTypeBits, properties));

	m_Memory = device->allocateMemory(allocInfo);

	device->bindBufferMemory(m_Buffer, m_Memory, 0);

	// Move data from the temporary buffer into the actual buffer
	buffer.copyTo(*this, command_pool);
}

Buffer::~Buffer()
{
	m_Device->destroyBuffer(m_Buffer);
	m_Device->freeMemory(m_Memory);
}

void* Buffer::map() const
{
	return m_Device->mapMemory(m_Memory, 0, VK_WHOLE_SIZE);
}

void Buffer::unmap() const
{
	m_Device->unmapMemory(m_Memory);
}
