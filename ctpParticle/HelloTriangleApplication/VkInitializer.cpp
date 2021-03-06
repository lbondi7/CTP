#include "VkInitializer.h"
#include "VkSetupHelper.h"


#include <stdexcept>

namespace VkInitializer
{
	void ImageView(VkDevice device, VkImage image, VkImageView& imageView, VkFormat format, VkImageAspectFlags aspectFlags) {

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

	VkVertexInputAttributeDescription VertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset) {
		VkVertexInputAttributeDescription vertexAttributeDescription = {};
		vertexAttributeDescription.binding = binding;
		vertexAttributeDescription.location = location;
		vertexAttributeDescription.format = format;
		vertexAttributeDescription.offset = offset;
		return vertexAttributeDescription;
	}

	VkVertexInputBindingDescription VertexBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) {
		VkVertexInputBindingDescription vertexAttributeDescription = {};
		vertexAttributeDescription.binding = binding;
		vertexAttributeDescription.stride = stride;
		vertexAttributeDescription.inputRate = inputRate;
		return vertexAttributeDescription;
	}

	VkDescriptorSetLayoutBinding DescriptorLayoutBinding(uint32_t binding, VkDescriptorType descriptorType, VkShaderStageFlags stageFlags, uint32_t descriptorCount, VkSampler* immutableSamplers)
	{
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = descriptorCount;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.pImmutableSamplers = immutableSamplers;
		layoutBinding.stageFlags = stageFlags;

		return layoutBinding;
	}

	VkDescriptorSetLayoutCreateInfo DescSetLayoutInfo(uint32_t BindingCount, const VkDescriptorSetLayoutBinding* bindings)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = BindingCount;
		layoutInfo.pBindings = bindings;

		return layoutInfo;
	}

	VkViewport Viewport(float xPos, float yPos, float width, float height, float minDepth, float maxDepth)
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

	VkRect2D ScissorRect(const VkOffset2D& offset, const VkExtent2D& extent)
	{
		VkRect2D scissorRect = {};
		scissorRect.offset = offset;
		scissorRect.extent = extent;
		return scissorRect;
	}

	VkPipelineViewportStateCreateInfo ViewportStateInfo(uint32_t viewportCount, uint32_t scissorCount, const VkViewport* viewports, const VkRect2D* scissors)
	{
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = viewportCount;
		viewportState.scissorCount = scissorCount;
		viewportState.pViewports = viewports;
		viewportState.pScissors = scissors;

		return viewportState;
	}

	VkPipelineRasterizationStateCreateInfo Rasteriser(VkBool32 depthClampEnabled, VkBool32 rasterizerDiscardEnabled,
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

	VkPipelineMultisampleStateCreateInfo MultiSampling(VkBool32 sampleShadingEnabled, VkSampleCountFlagBits rasterisationSamples)
	{
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = sampleShadingEnabled;
		multisampling.rasterizationSamples = rasterisationSamples;

		return multisampling;
	}

	VkPipelineColorBlendAttachmentState ColourBlendAttachment(VkColorComponentFlags colourWriteMask, VkBool32 blendEnabled)
	{
		VkPipelineColorBlendAttachmentState colourBlendAttachment = {};
		colourBlendAttachment.colorWriteMask = colourWriteMask;
		colourBlendAttachment.blendEnable = blendEnabled;

		return colourBlendAttachment;
	}

	VkPipelineColorBlendStateCreateInfo ColourBlendStateInfo(VkBool32 logicOpEnabled, VkLogicOp logicOp,
		uint32_t attachmentCount, const VkPipelineColorBlendAttachmentState* colourBlendAttachment, const std::vector<float>& blendConstants)
	{
		VkPipelineColorBlendStateCreateInfo colourBlending = {};
		colourBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
		//colourBlending.logicOpEnable = logicOpEnabled;
		//colourBlending.logicOp = logicOp;
		colourBlending.attachmentCount = attachmentCount;
		colourBlending.pAttachments = colourBlendAttachment;
		//colourBlending.blendConstants[0] = blendConstants[0];
		//colourBlending.blendConstants[1] = blendConstants[1];
		//colourBlending.blendConstants[2] = blendConstants[2];
		//colourBlending.blendConstants[3] = blendConstants[3];

		return colourBlending;
	}

	VkPipelineDepthStencilStateCreateInfo DepthStencilInfo(VkBool32 depthTestEnabled, VkBool32 depthWriteEnabled,
		VkCompareOp depthCompareOp, VkBool32 depthBoundsTestEnabled, VkBool32 stencilTestEnabled)
	{
		VkPipelineDepthStencilStateCreateInfo depthStencil = {};
		depthStencil.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
		depthStencil.depthTestEnable = depthTestEnabled;
		depthStencil.depthWriteEnable = depthWriteEnabled;
		depthStencil.depthCompareOp = depthCompareOp;
		//depthStencil.depthBoundsTestEnable = depthBoundsTestEnabled;
		//depthStencil.stencilTestEnable = stencilTestEnabled;

		return depthStencil;
	}

	VkDescriptorPoolSize DescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = type;
		poolSize.descriptorCount = descriptorCount;

		return poolSize;
	}

	VkDescriptorPoolCreateInfo DescriptorPoolInfo(uint32_t poolSizeCount, const VkDescriptorPoolSize* poolSizeData, uint32_t maxDescriptorSets)
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

	VkWriteDescriptorSet WriteDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorBufferInfo* bufferInfo, uint32_t descriptorCount)
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

	VkWriteDescriptorSet WriteDescSet(VkDescriptorSet dstSet, VkDescriptorType type, uint32_t binding, VkDescriptorImageInfo* imageInfo, uint32_t descriptorCount)
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