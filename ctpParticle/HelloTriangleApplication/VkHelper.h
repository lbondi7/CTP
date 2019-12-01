#pragma once

#include "VkConstants.h"

namespace VkHelper
{
	void createBufferWithStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const void* data);

	void createBufferWithoutStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void createBuffer(const VkDevice& device, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkBuffer& buffer);

	void AllocateBufferMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory);

	void copyMemory(const VkDevice& device, VkDeviceSize bufferSize, VkDeviceMemory& bufferMemory, const void* data);

	void copyBuffer(const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size);

	uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties);

	VkCommandBuffer beginSingleTimeCommands(const VkDevice& device, VkCommandBufferLevel cmdBufferLevel, VkCommandPool& cmdPool, int cmdBufferCount);

	void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkDevice& device, VkCommandPool& cmdPool, int cmdBufferCount, const VkQueue graphicsQueue);

	const VkVertexInputAttributeDescription& createVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset);

	const VkVertexInputBindingDescription& createVertexBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate);

	const VkDescriptorSetLayoutBinding& createDescriptorLayoputBinding(uint32_t binding, uint32_t descriptorCount, VkDescriptorType descriptorType, VkSampler* immutableSamplers, VkShaderStageFlags stageFlags);

	const VkDescriptorSetLayoutCreateInfo& createDescSetLayoutInfo(uint32_t BindingCount, const VkDescriptorSetLayoutBinding* bindings);

	const VkPipelineShaderStageCreateInfo& createShaderStageInfo(VkStructureType structureType, VkShaderStageFlagBits stage, const VkShaderModule& shaderModule, const char* name);

	const VkViewport& createViewport(float xPos, float yPos, float width, float height, float minDepth, float maxDepth);

	const VkRect2D& createScissorRect(const VkOffset2D& offset, const VkExtent2D& extent);

	const VkPipelineViewportStateCreateInfo& createViewPortStateInfo(uint32_t viewportCount, uint32_t scissorCount, const VkViewport* viewports, const VkRect2D* scissors);

	const VkPipelineRasterizationStateCreateInfo& createRasteriser(VkBool32 depthClampEnabled, VkBool32 rasterizerDiscardEnabled,
		VkPolygonMode polygonMode, float lineWidth, VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBiasEnabled);

	const VkPipelineMultisampleStateCreateInfo& createMultiSampling(VkBool32 sampleShadingEnabled, VkSampleCountFlagBits rasterisationSamples);

	const VkPipelineColorBlendAttachmentState& createColourBlendAttachment(VkColorComponentFlags colourWriteMask, VkBool32 blendEnabled);

	const VkPipelineColorBlendStateCreateInfo& createColourBlendStateInfo(VkBool32 logicOpEnabled, VkLogicOp logicOp, 
		uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* colourBlendAttachment, const std::vector<float>& blendConstants);

	const VkPipelineDepthStencilStateCreateInfo& createDepthStencilInfo(VkBool32 depthTestEnabled, VkBool32 depthWriteEnabled, VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnabled, VkBool32 stencilTestEnabled);

	const VkDescriptorPoolSize& createDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount);

	const VkDescriptorPoolCreateInfo& createDescriptorPoolInfo(uint32_t poolSizeCount, const VkDescriptorPoolSize* poolSizeData, uint32_t maxDescriptorSets);
};

