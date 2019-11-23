#pragma once

#include "VkConstants.h"

struct Image
{
	//stbi_uc* pixels;
	void* pixels;
	VkDeviceSize imageSize;
	int texWidth, texHeight, texChannels;

	void loadImage(const char* imagePath);
	void freeImage();
};

