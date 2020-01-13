#include "VkHelper.h"

#include <stdexcept>

namespace VkHelper
{
	void createBufferWithStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory, const void* data) {

		VkBuffer stagingBuffer;
		VkDeviceMemory stagingBufferMemory;
		createBuffer(device, bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, stagingBuffer);
		AllocateBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingBufferMemory);

		copyMemory(device, bufferSize, stagingBufferMemory, data);

		createBuffer(device, bufferSize, usage, buffer);
		AllocateBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);

		copyBuffer(device, cmdPool, graphicsQueue, stagingBuffer, buffer, bufferSize);

		vkDestroyBuffer(device, stagingBuffer, nullptr);
		vkFreeMemory(device, stagingBufferMemory, nullptr);
	}

	void createBufferWithoutStaging(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkBufferUsageFlags usage, VkDeviceSize bufferSize, VkBuffer& buffer, VkDeviceMemory& bufferMemory) {

		createBuffer(device, bufferSize, usage, buffer);
		AllocateBufferMemory(physicalDevice, device, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, buffer, bufferMemory);
	}

	void createBuffer(const VkDevice& device, VkDeviceSize bufferSize, VkBufferUsageFlags usage, VkBuffer& buffer) {

		VkBufferCreateInfo bufferInfo = {};
		bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
		bufferInfo.size = bufferSize;
		bufferInfo.usage = usage;
		bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;

		if (vkCreateBuffer(device, &bufferInfo, nullptr, &buffer) != VK_SUCCESS) {
			throw std::runtime_error("failed to create buffer!");
		}
	}

	void AllocateBufferMemory(const VkPhysicalDevice& physicalDevice, const VkDevice& device, VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& bufferMemory)
	{
		VkMemoryRequirements memRequirements;
		vkGetBufferMemoryRequirements(device, buffer, &memRequirements);

		VkMemoryAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
		allocInfo.allocationSize = memRequirements.size;
		allocInfo.memoryTypeIndex = findMemoryType(physicalDevice, memRequirements.memoryTypeBits, properties);

		if (vkAllocateMemory(device, &allocInfo, nullptr, &bufferMemory) != VK_SUCCESS) {
			throw std::runtime_error("failed to allocate buffer memory!");
		}

		vkBindBufferMemory(device, buffer, bufferMemory, 0);
	}

	void copyMemory(const VkDevice& device, VkDeviceSize bufferSize, VkDeviceMemory& bufferMemory, const void* data)
	{
		void* _data;
		vkMapMemory(device, bufferMemory, 0, bufferSize, 0, &_data);
		memcpy(_data, data, (size_t)bufferSize);
		vkUnmapMemory(device, bufferMemory);
	}

	void copyBuffer(const VkDevice& device, VkCommandPool cmdPool, VkQueue graphicsQueue, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size) {
		VkCommandBuffer commandBuffer = beginSingleTimeCommands(device, VK_COMMAND_BUFFER_LEVEL_PRIMARY, cmdPool, 1);

		VkBufferCopy copyRegion = {};
		copyRegion.size = size;
		vkCmdCopyBuffer(commandBuffer, srcBuffer, dstBuffer, 1, &copyRegion);

		endSingleTimeCommands(commandBuffer, device, cmdPool, 1, graphicsQueue);
	}

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

	VkCommandBuffer beginSingleTimeCommands(const VkDevice& device, VkCommandBufferLevel cmdBufferLevel, VkCommandPool& cmdPool, int cmdBufferCount) {
		VkCommandBufferAllocateInfo allocInfo = {};
		allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
		allocInfo.level = cmdBufferLevel;
		allocInfo.commandPool = cmdPool;
		allocInfo.commandBufferCount = cmdBufferCount;

		VkCommandBuffer commandBuffer;
		vkAllocateCommandBuffers(device, &allocInfo, &commandBuffer);

		VkCommandBufferBeginInfo beginInfo = {};
		beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
		beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

		vkBeginCommandBuffer(commandBuffer, &beginInfo);

		return commandBuffer;
	}

	void endSingleTimeCommands(VkCommandBuffer commandBuffer, const VkDevice& device, VkCommandPool& cmdPool, int cmdBufferCount, const VkQueue graphicsQueue) {
		vkEndCommandBuffer(commandBuffer);

		VkSubmitInfo submitInfo = {};
		submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
		submitInfo.commandBufferCount = cmdBufferCount;
		submitInfo.pCommandBuffers = &commandBuffer;

		vkQueueSubmit(graphicsQueue, 1, &submitInfo, VK_NULL_HANDLE);
		vkQueueWaitIdle(graphicsQueue);

		vkFreeCommandBuffers(device, cmdPool, cmdBufferCount, &commandBuffer);
	}

	const VkVertexInputAttributeDescription& createVertexAttributeDescription(uint32_t binding, uint32_t location, VkFormat format, uint32_t offset) {
		VkVertexInputAttributeDescription vertexAttributeDescription = {};
		vertexAttributeDescription.binding = binding;
		vertexAttributeDescription.location = location;
		vertexAttributeDescription.format = format;
		vertexAttributeDescription.offset = offset;
		return vertexAttributeDescription;
	}

	const VkVertexInputBindingDescription& createVertexBindingDescription(uint32_t binding, uint32_t stride, VkVertexInputRate inputRate) {
		VkVertexInputBindingDescription vertexAttributeDescription = {};
		vertexAttributeDescription.binding = binding;
		vertexAttributeDescription.stride = stride;
		vertexAttributeDescription.inputRate = inputRate;
		return vertexAttributeDescription;
	}

	const VkDescriptorSetLayoutBinding& createDescriptorLayoputBinding(uint32_t binding, uint32_t descriptorCount, VkDescriptorType descriptorType, VkSampler* immutableSamplers, VkShaderStageFlags stageFlags)
	{
		VkDescriptorSetLayoutBinding layoutBinding = {};
		layoutBinding.binding = binding;
		layoutBinding.descriptorCount = descriptorCount;
		layoutBinding.descriptorType = descriptorType;
		layoutBinding.pImmutableSamplers = immutableSamplers;
		layoutBinding.stageFlags = stageFlags;

		return layoutBinding;
	}

	const VkDescriptorSetLayoutCreateInfo& createDescSetLayoutInfo(uint32_t BindingCount, const VkDescriptorSetLayoutBinding* bindings)
	{
		VkDescriptorSetLayoutCreateInfo layoutInfo = {};
		layoutInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
		layoutInfo.bindingCount = BindingCount;
		layoutInfo.pBindings = bindings;

		return layoutInfo;
	}

	const VkPipelineShaderStageCreateInfo& createShaderStageInfo(VkStructureType structureType, VkShaderStageFlagBits stage, const VkShaderModule& shaderModule, const char* name)
	{
		VkPipelineShaderStageCreateInfo shaderStageInfo = {};
		shaderStageInfo.sType = structureType;
		shaderStageInfo.stage = stage;
		shaderStageInfo.module = shaderModule;
		shaderStageInfo.pName = name;

		return shaderStageInfo;
	}

	const VkViewport& createViewport(float xPos, float yPos, float width, float height, float minDepth, float maxDepth)
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

	const VkRect2D& createScissorRect(const VkOffset2D& offset, const VkExtent2D& extent)
	{
		VkRect2D scissorRect = {};
		scissorRect.offset = offset;
		scissorRect.extent = extent;
		return scissorRect;
	}

	const VkPipelineViewportStateCreateInfo& createViewPortStateInfo(uint32_t viewportCount, uint32_t scissorCount, const VkViewport* viewports, const VkRect2D* scissors)
	{
		VkPipelineViewportStateCreateInfo viewportState = {};
		viewportState.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
		viewportState.viewportCount = viewportCount;
		viewportState.scissorCount = scissorCount;
		viewportState.pViewports = viewports;
		viewportState.pScissors = scissors;

		return viewportState;
	}

	const VkPipelineRasterizationStateCreateInfo& createRasteriser(VkBool32 depthClampEnabled, VkBool32 rasterizerDiscardEnabled, 
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

	const VkPipelineMultisampleStateCreateInfo& createMultiSampling(VkBool32 sampleShadingEnabled, VkSampleCountFlagBits rasterisationSamples)
	{
		VkPipelineMultisampleStateCreateInfo multisampling = {};
		multisampling.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
		multisampling.sampleShadingEnable = sampleShadingEnabled;
		multisampling.rasterizationSamples = rasterisationSamples;

		return multisampling;
	}

	const VkPipelineColorBlendAttachmentState& createColourBlendAttachment(VkColorComponentFlags colourWriteMask, VkBool32 blendEnabled)
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

	const VkPipelineColorBlendStateCreateInfo& createColourBlendStateInfo(VkBool32 logicOpEnabled, VkLogicOp logicOp, 
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

	const VkPipelineDepthStencilStateCreateInfo& createDepthStencilInfo(VkBool32 depthTestEnabled, VkBool32 depthWriteEnabled,
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

	const VkDescriptorPoolSize& createDescriptorPoolSize(VkDescriptorType type, uint32_t descriptorCount)
	{
		VkDescriptorPoolSize poolSize = {};
		poolSize.type = type;
		poolSize.descriptorCount = descriptorCount;

		return poolSize;
	}

	const VkDescriptorPoolCreateInfo& createDescriptorPoolInfo(uint32_t poolSizeCount, const VkDescriptorPoolSize* poolSizeData, uint32_t maxDescriptorSets)
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