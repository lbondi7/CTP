#include "VkInitializer.h"

namespace VkInitializers
{

	VkDescriptorBufferInfo DescBufferInfo(VkBuffer buffer, VkDeviceSize range, VkDeviceSize offset)
	{
		VkDescriptorBufferInfo bufferInfo = {};
		bufferInfo.buffer = buffer;
		bufferInfo.offset = offset;
		bufferInfo.range = range;
		return bufferInfo;
	}

	VkDescriptorImageInfo DescImageInfo(VkImageLayout imageLayout, VkImageView imageView, VkSampler sampler)
	{
		VkDescriptorImageInfo imageInfo = {};
		imageInfo.imageLayout = imageLayout;
		imageInfo.imageView = imageView;
		imageInfo.sampler = sampler;
		return imageInfo;
	}



};