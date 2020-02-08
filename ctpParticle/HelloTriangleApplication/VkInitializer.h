#pragma once

#include "VkConstants.h"

namespace VkInitializers
{
	VkDescriptorBufferInfo DescBufferInfo(VkBuffer buffer, VkDeviceSize range, VkDeviceSize offset = 0);

	VkDescriptorImageInfo DescImageInfo(VkImageLayout imageLayout, VkImageView imageView, VkSampler sampler);
};

