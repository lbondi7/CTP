#pragma once

#include "VkConstants.h"

#include <map>
#include <string>

struct Image
{
	std::vector<void*> pixels;
	std::vector<VkDeviceSize> imageSize;
	std::vector<glm::vec3> textureDetails;

	std::map<std::string, int> images;
	int imageCount = 0;

	void Load(const std::string& texture);
};

