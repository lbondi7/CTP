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
	VkImageLayout imageLayout;
	VkDescriptorImageInfo descriptor;

	void* pixels;
	VkDeviceSize size;
	int texWidth, texHeight, texChannels;

	Texture() = default;

	~Texture();

	void Load(const std::string imagePath, VkQueue queue, VkFormat format, VkMemoryPropertyFlags _memProperties, VkImageUsageFlags usage);

	void createImage(VkFormat format, VkImageUsageFlags usage);

	void AllocateImageMemory(VkMemoryPropertyFlags properties);

	void transitionImageLayout(VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout, VkQueue queue);

	void CopyBufferToImage(VkBuffer buffer, VkQueue queue);

	void CreateTextureSampler();

	void Destroy();
};

