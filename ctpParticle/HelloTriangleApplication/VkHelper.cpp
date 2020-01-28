#include "VkHelper.h"
#include "VkSetupHelper.h"

#include <stdexcept>

namespace VkHelper
{
	uint32_t findMemoryType(const VkPhysicalDevice& physicalDevice, uint32_t typeFilter, VkMemoryPropertyFlags properties) {
		VkPhysicalDeviceMemoryProperties memProperties;
		vkGetPhysicalDeviceMemoryProperties(physicalDevice, &memProperties);

		for (uint32_t i = 0; i < memProperties.memoryTypeCount; i++) {
			if ((typeFilter & (1 << i)) && (memProperties.memoryTypes[i].propertyFlags & properties) == properties) {
				return i;
			}
		}

		throw std::runtime_error("failed to find suitable memory type!");
	}

	void CreateImageView(VkDevice device, VkImage image, VkImageView& imageView, VkFormat format, VkImageAspectFlags aspectFlags) {

		VkImageViewCreateInfo viewInfo = {};
		viewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
		viewInfo.image = image;
		viewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
		viewInfo.format = format;
		viewInfo.subresourceRange.aspectMask = aspectFlags;
		viewInfo.subresourceRange.baseMipLevel = 0;
		viewInfo.subresourceRange.levelCount = 1;
		viewInfo.subresourceRange.baseArrayLayer = 0;
		viewInfo.subresourceRange.layerCount = 1;

		if (vkCreateImageView(device, &viewInfo, nullptr, &imageView) != VK_SUCCESS) {
			throw std::runtime_error("failed to create texture image view!");
		}
	}

	bool HasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkVertexInputAttributeDescription createVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset) {
		VkVertexInputAttributeDescription vertexAttributeDescription = {};
		vertexAttributeDescription.binding = binding;
		vertexAttributeDescription.location = location;
		vertexAttributeDescription.format = format;
		vertexAttributeDescription.offset = offset;
		return vertexAttributeDescription;
	}

	VkVertexInputBindingDescription createVertexBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) {
		VkVertexInputBindingDescription vertexAttributeDescription = {};
		vertexAttributeDescription.binding = binding;
		vertexAttributeDescription.stride = stride;
		vertexAttributeDescription.inputRate = inputRate;
		return vertexAttributeDescription;
	}

	VkDescriptorSetLayoutBinding createDescriptorLayoutBinding(uint32_t binding, uint32_t descriptorCount, VkDescriptorType descriptorType, VkSampler* immutableSamplers, VkShaderStageFlags stageFlags)
	{
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = descriptorCount;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.pImmutableSamplers = immutableSamplers;
		layoutBinding.stageFlags = stageFlags;

		return layoutBinding;
	}

	VkDescriptorSetLayoutCreateInfo createDescSetLayoutInfo(uint32_t BindingCount, const VkDescriptorSetLayoutBinding* bindings)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = BindingCount;
		layoutInfo.pBindings = bindings;

		return layoutInfo;
	}

	VkPipelineShaderStageCreateInfo createShaderStageInfo(VkStructureType structureType, VkShaderStageFlagBits stage, const VkShaderModule& shaderModule, const char* name)
	{



		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		shaderStageInfo.sType = structureType;
		shaderStageInfo.stage = stage;
		shaderStageInfo.module = shaderModule;
		shaderStageInfo.pName = name;

		return shaderStageInfo;
	}

	VkPipelineShaderStageCreateInfo createShaderStageInfo(std::string filepath, VkShaderStageFlagBits stage, VkDevice device)
	{
		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		shaderStageInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
		shaderStageInfo.stage = stage;
		shaderStageInfo.module = VkSetupHelper::createShaderModule(filepath, device);
		shaderStageInfo.pName = "main";

		return shaderStageInfo;
	}

	VkViewport createViewport(float xPos, float yPos, float width, float height, float minDepth, float maxDepth)
	{
		VkViewport viewport = {};
		viewport.x = xPos;
		viewport.y = yPos;
		viewport.width = width;
		viewport.height = height;
		viewport.minDepth = minDepth;
		viewport.maxDepth = maxDepth;
		
		return viewport;
	}

	VkRect2D createScissorRect(const VkOffset2D& offset, const VkExtent2D& extent)
	{
		VkRect2D scissorRect = {};
		scissorRect.offset = offset;
		scissorRect.extent = extent;
		return scissorRect;
	}

	VkPipelineViewportStateCreateInfo createViewPortStateInfo(uint32_t viewportCount, uint32_t scissorCount, const VkViewport* viewports, const VkRect2D* scissors)
	{
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = viewportCount;
		viewportState.scissorCount = scissorCount;
		viewportState.pViewports = viewports;
		viewportState.pScissors = scissors;

		return viewportState;
	}

	VkPipelineRasterizationStateCreateInfo createRasteriser(VkBool32 depthClampEnabled, VkBool32 rasterizerDiscardEnabled, 
		VkPolygonMode polygonMode, float lineWidth, VkCullModeFlags cullMode, VkFrontFace frontFace, VkBool32 depthBiasEnabled)
	{
		VkPipelineRasterizationStateCreateInfo rasterizer = {};
		rasterizer.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
		rasterizer.depthClampEnable = depthClampEnabled;
		rasterizer.rasterizerDiscardEnable = rasterizerDiscardEnabled;
		rasterizer.polygonMode = polygonMode;
		rasterizer.lineWidth = lineWidth;
		rasterizer.cullMode = cullMode;
		rasterizer.frontFace = frontFace;
		rasterizer.depthBiasEnable = depthBiasEnabled;

		return rasterizer;
	}

	VkPipelineMultisampleStateCreateInfo createMultiSampling(VkBool32 sampleShadingEnabled, VkSampleCountFlagBits rasterisationSamples)
	{
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = sampleShadingEnabled;
		multisampling.rasterizationSamples = rasterisationSamples;

		return multisampling;
	}

	VkPipelineColorBlendAttachmentState createColourBlendAttachment(VkColorComponentFlags colourWriteMask, VkBool32 blendEnabled)
	{
		VkPipelineColorBlendAttachmentState colourBlendAttachment = {};
		colourBlendAttachment.colorWriteMask = colourWriteMask;
		colourBlendAttachment.blendEnable = blendEnabled;
		//colourBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		//colourBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		//colourBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD; // Optional
		//colourBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE; // Optional
		//colourBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO; // Optional
		//colourBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD; // Optional

		return colourBlendAttachment;
	}

	VkPipelineColorBlendStateCreateInfo createColourBlendStateInfo(VkBool32 logicOpEnabled, VkLogicOp logicOp, 
		uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* colourBlendAttachment, const std::vector<float>& blendConstants)
	{
		VkPipelineColorBlendStateCreateInfo colourBlending = {};
		colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		colourBlending.logicOpEnable = logicOpEnabled;
		colourBlending.logicOp = logicOp;
		colourBlending.attachmentCount = attachmentCount;
		colourBlending.pAttachments = colourBlendAttachment;
		colourBlending.blendConstants[0] = blendConstants[0];
		colourBlending.blendConstants[1] = blendConstants[1];
		colourBlending.blendConstants[2] = blendConstants[2];
		colourBlending.blendConstants[3] = blendConstants[3];

		return colourBlending;
	}

	 VkPipelineDepthStencilStateCreateInfo createDepthStencilInfo(VkBool32 depthTestEnabled, VkBool32 depthWriteEnabled,
		VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnabled, VkBool32 stencilTestEnabled)
	{
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = depthTestEnabled;
		depthStencil.depthWriteEnable = depthWriteEnabled;
		depthStencil.depthCompareOp = depthCompareOp;
		depthStencil.depthBoundsTestEnable = depthBoundsTestEnabled;
		depthStencil.stencilTestEnable = stencilTestEnabled;


		return depthStencil;
	}

	VkDescriptorPoolSize createDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = type;
		poolSize.descriptorCount = descriptorCount;

		return poolSize;
	}

	VkDescriptorPoolCreateInfo createDescriptorPoolInfo(uint32_t poolSizeCount, const VkDescriptorPoolSize* poolSizeData, uint32_t maxDescriptorSets)
	{
		VkDescriptorPoolCreateInfo poolInfo = {};
		poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
		poolInfo.poolSizeCount = poolSizeCount;
		poolInfo.pPoolSizes = poolSizeData;
		poolInfo.maxSets = maxDescriptorSets;
		return poolInfo;
	}

	bool hasStencilComponent(VkFormat format) {
		return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
	}

	VkWriteDescriptorSet writeDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo* bufferInfo, uint32_t descriptorCount)
	{
		VkWriteDescriptorSet descriptorWrite = {};

		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = dstSet;
		descriptorWrite.dstBinding = binding;
		descriptorWrite.descriptorType = type;
		descriptorWrite.descriptorCount = descriptorCount;
		descriptorWrite.pBufferInfo = bufferInfo;

		return descriptorWrite;
	}

	VkWriteDescriptorSet writeDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* imageInfo, uint32_t descriptorCount)
	{
		VkWriteDescriptorSet descriptorWrite = {};

		descriptorWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
		descriptorWrite.dstSet = dstSet;
		descriptorWrite.dstBinding = binding;
		descriptorWrite.descriptorType = type;
		descriptorWrite.descriptorCount = descriptorCount;
		descriptorWrite.pImageInfo = imageInfo;

		return descriptorWrite;
	}
};