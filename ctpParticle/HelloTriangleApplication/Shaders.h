#pragma once

#include "VkConstants.h"

#include <string>
#include <map>

struct Shaders
{
	VkPipelineShaderStageCreateInfo CreateShaderInfo(std::string filepath, VkShaderStageFlagBits stage);

	void CreateShaderModuleInfo(std::string filePath);

	std::vector<char> readFile(const std::string& filename);

	void DestroyShaders();

	VkShaderModule shader;
	std::vector<VkShaderModule> shaders;
	std::map<std::string, int> shaderMap;
	int shaderCount;

};

