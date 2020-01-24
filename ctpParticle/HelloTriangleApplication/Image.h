#pragma once

#include "VkConstants.h"

struct Image
{
	void* pixels;
	VkDeviceSize imageSize;
	int texWidth, texHeight, texChannels;

	void loadImage(const char* imagePath);
	void freeImage();
};

