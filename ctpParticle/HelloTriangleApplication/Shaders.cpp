#include "Shaders.h"
#include "Locator.h"
#include "Devices.h"

#include <iostream>
#include <stdexcept>
#include <fstream>

VkPipelineShaderStageCreateInfo Shaders::CreateShaderInfo(std::string filepath, VkShaderStageFlagBits stage)
{
	shaderMap[filepath] = shaderCount;
	shaderCount++;
	shaders.resize(shaderCount);

	VkPipelineShaderStageCreateInfo shaderStageInfo = {};
	shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
	shaderStageInfo.stage = stage;
	shaderStageInfo.pName = "main";

	CreateShaderModuleInfo(filepath);


	shaderStageInfo.module = shaders[shaderMap[filepath]];

	return shaderStageInfo;
}

void Shaders::CreateShaderModuleInfo(std::string filepath) {

	auto shaderCode = readFile(filepath);

	VkShaderModuleCreateInfo createInfo = {};
	createInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
	createInfo.codeSize = shaderCode.size();
	createInfo.pCode = reinterpret_cast<const uint32_t*>(shaderCode.data());

	if (vkCreateShaderModule(Locator::GetDevices()->GetDevice(), &createInfo, nullptr, &shaders[shaderMap[filepath]]) != VK_SUCCESS) {
		throw std::runtime_error("failed to create shader module!");
	}

}

std::vector<char> Shaders::readFile(const std::string& filepath) {

	std::string filename = filepath;

	filename.resize(filename.size() - 4);

	std::ifstream file("shaders/" + filename + "/" + filepath + ".spv", std::ios::ate | std::ios::binary);

	if (!file.is_open()) {
		throw std::runtime_error("failed to open file!");
	}

	size_t fileSize = (size_t)file.tellg();
	std::vector<char> buffer(fileSize);

	file.seekg(0);
	file.read(buffer.data(), fileSize);

	file.close();

	return buffer;
}

void Shaders::DestroyShaders()
{
	for (size_t i = 0; i < shaderCount; i++)
	{
		vkDestroyShaderModule(Locator::GetDevices()->GetDevice(), shaders[i], nullptr);
	}
	shaderCount = 0;
	shaderMap.clear();
}
