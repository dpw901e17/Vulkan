#pragma once
#include <vulkan/vulkan.hpp>

class Shader
{
public:
	Shader(vk::Device device, std::string file_path, vk::ShaderStageFlagBits stage);
	Shader(const Shader&) = delete;
	~Shader();

	Shader& operator=(const Shader&) = delete;
	vk::PipelineShaderStageCreateInfo info() const { return m_Info; }
private:
	vk::PipelineShaderStageCreateInfo m_Info;
	vk::Device m_Device;
	vk::ShaderModule m_Module;
};
