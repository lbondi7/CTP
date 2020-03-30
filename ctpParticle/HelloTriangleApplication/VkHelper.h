#pragma once
#include "VkConstants.h"

#include <string>

namespace VkHelper
{
	uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	void CreateImageView(VkDevice device, VkImage image, VkImageView& imageView, VkFormat format, VkImageAspectFlags aspectFlags);

	bool HasStencilComponent(VkFormat format);

	VkVertexInputAttributeDescription createVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

	VkVertexInputBindingDescription createVertexBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

	VkDescriptorSetLayoutBinding createDescriptorLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount = 1U, VkSampler* immutableSamplers = nullptr);

	VkDescriptorSetLayoutCreateInfo createDescSetLayoutInfo(uint32_t BindingCount, const VkDescriptorSetLayoutBinding* bindings);

	VkPipelineShaderStageCreateInfo createShaderStageInfo(VkStructureType structureType, VkShaderStageFlagBits stage, const VkShaderModule& shaderModule, const char* name);

	VkPipelineShaderStageCreateInfo createShaderStageInfo(std::string filepath, VkShaderStageFlagBits stage, VkDevice device);

	VkViewport createViewport(float xPos, float yPos, float width, float height, float minDepth, float maxDepth);

	VkRect2D createScissorRect(const VkOffset2D& offset, const VkExtent2D& extent);

	VkPipelineViewportStateCreateInfo createViewPortStateInfo(uint32_t viewportCount, uint32_t scissorCount, const VkViewport* viewports, const VkRect2D* scissors);

	VkPipelineRasterizationStateCreateInfo createRasteriser(VkBool32 depthClampEnabled, VkBool32 rasterizerDiscardEnabled,
		VkPolygonMode polygonMode, float lineWidth, VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBiasEnabled);

	VkPipelineMultisampleStateCreateInfo createMultiSampling(VkBool32 sampleShadingEnabled, VkSampleCountFlagBits rasterisationSamples);

	VkPipelineColorBlendAttachmentState createColourBlendAttachment(VkColorComponentFlags colourWriteMask, VkBool32 blendEnabled);

	VkPipelineColorBlendStateCreateInfo createColourBlendStateInfo(VkBool32 logicOpEnabled, VkLogicOp logicOp, 
		uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* colourBlendAttachment, const std::vector<float>& blendConstants);

	VkPipelineDepthStencilStateCreateInfo createDepthStencilInfo(VkBool32 depthTestEnabled, VkBool32 depthWriteEnabled, VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnabled, VkBool32 stencilTestEnabled);

	VkDescriptorPoolSize createDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount);

	VkDescriptorPoolCreateInfo createDescriptorPoolInfo(uint32_t poolSizeCount, const VkDescriptorPoolSize* poolSizeData, uint32_t maxDescriptorSets);
	
	bool hasStencilComponent(VkFormat format);

	VkWriteDescriptorSet writeDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo* bufferInfo, uint32_t descriptorCount = 1);

	VkWriteDescriptorSet writeDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* imageInfo, uint32_t descriptorCount = 1);


};

