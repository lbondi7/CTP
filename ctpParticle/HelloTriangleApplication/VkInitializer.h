#pragma once

#include "VkConstants.h"

namespace VkInitializer
{
	void ImageView(VkDevice device, VkImage image, VkImageView& imageView, VkFormat format, VkImageAspectFlags aspectFlags);

	bool HasStencilComponent(VkFormat format);

	VkVertexInputAttributeDescription VertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

	VkVertexInputBindingDescription VertexBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

	VkDescriptorSetLayoutBinding DescriptorLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount = 1U, VkSampler* immutableSamplers = nullptr);

	VkDescriptorSetLayoutCreateInfo DescSetLayoutInfo(uint32_t BindingCount, const VkDescriptorSetLayoutBinding* bindings);

	VkViewport Viewport(float xPos, float yPos, float width, float height, float minDepth, float maxDepth);

	VkRect2D ScissorRect(const VkOffset2D& offset, const VkExtent2D& extent);

	VkPipelineViewportStateCreateInfo ViewportStateInfo(uint32_t viewportCount, uint32_t scissorCount, const VkViewport* viewports, const VkRect2D* scissors);

	VkPipelineRasterizationStateCreateInfo Rasteriser(VkBool32 depthClampEnabled, VkBool32 rasterizerDiscardEnabled,
		VkPolygonMode polygonMode, float lineWidth, VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBiasEnabled);

	VkPipelineMultisampleStateCreateInfo MultiSampling(VkBool32 sampleShadingEnabled, VkSampleCountFlagBits rasterisationSamples);

	VkPipelineColorBlendAttachmentState ColourBlendAttachment(VkColorComponentFlags colourWriteMask, VkBool32 blendEnabled);

	VkPipelineColorBlendStateCreateInfo ColourBlendStateInfo(VkBool32 logicOpEnabled, VkLogicOp logicOp,
		uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* colourBlendAttachment, const std::vector<float>& blendConstants);

	VkPipelineDepthStencilStateCreateInfo DepthStencilInfo(VkBool32 depthTestEnabled, VkBool32 depthWriteEnabled, VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnabled, VkBool32 stencilTestEnabled);

	VkDescriptorPoolSize DescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount);

	VkDescriptorPoolCreateInfo DescriptorPoolInfo(uint32_t poolSizeCount, const VkDescriptorPoolSize* poolSizeData, uint32_t maxDescriptorSets);

	VkWriteDescriptorSet WriteDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo* bufferInfo, uint32_t descriptorCount = 1);

	VkWriteDescriptorSet WriteDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* imageInfo, uint32_t descriptorCount = 1);
};

