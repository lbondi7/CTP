#pragma once

#include "VkConstants.h"
#include "Image.h"

class Texture
{
public:
	Texture() = default;
	~Texture();

	void createTexture(const VkDevice& device, const VkPhysicalDevice& physicalDevice);

	void DestroyTexture(VkDevice device);

	void createTextureImage(const VkDevice& device, const VkPhysicalDevice& physicalDevice);

	void createImage(const VkDevice& device, uint32_t width, uint32_t height, VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkImage& image);

	void transitionImageLayout(const VkDevice& device, const VkPhysicalDevice& physicalDevic, VkImage image, VkFormat format, VkImageLayout oldLayout, VkImageLayout newLayout);

	void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

	void createTextureImageView();

	void createTextureSampler(const VkDevice& device);

	void AllocateImageMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkImage& image, VkDeviceMemory& imageMemory);

private:

	Image image;
	VkImage textureImage;
	VkDeviceMemory textureImageMemory;
	VkImageView textureImageView;
	VkSampler textureSampler;
};

