#pragma once

#include "VkConstants.h"
#include "Image.h"

struct Texture
{

	VkDevice device;

	VkImage image;
	VkDeviceMemory memory;
	VkImageView imageView;
	VkSampler sampler;

	void* pixels;
	VkDeviceSize size;
	int texWidth, texHeight, texChannels;

	~Texture();

	void Load(const char* imagePath, VkQueue queue, VkFormat format, VkMemoryPropertyFlags _memProperties, VkImageUsageFlags usage);

	void createImage(VkFormat format, VkImageUsageFlags usage);

	void AllocateImageMemory(VkMemoryPropertyFlags properties);

	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkQueue queue);

	bool hasStencilComponent(VkFormat format);

	void CopyBufferToImage(VkBuffer buffer, VkQueue queue);

	void CreateImageView(VkFormat format, VkImageAspectFlags aspectFlags);

	void CreateTextureSampler();



};

