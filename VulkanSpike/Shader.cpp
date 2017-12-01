#include "Shader.h"

VkShaderModule createShaderModule(VkDevice device, const std::vector<char>& code)
{
	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = code.size();
	//.pCode requires a uint32_t* but "code.data()" is a char* !
	createInfo.pCode = reinterpret_cast<const uint32_t*>(code.data());

	VkShaderModule shaderModule;
	if (vkCreateShaderModule(device, &createInfo, nullptr, &shaderModule) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

	return shaderModule;
}

Shader::Shader(VkDevice device, std::string file_path, VkShaderStageFlagBits stage)
	: m_Device(device)
{
	auto source = readFile(file_path);
	m_Module = createShaderModule(device, source);

	m_Info.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	m_Info.stage = stage;
	m_Info.module = m_Module;
	m_Info.pName = "main";
}

Shader::~Shader()
{
	vkDestroyShaderModule(m_Device, m_Module, nullptr);
}
