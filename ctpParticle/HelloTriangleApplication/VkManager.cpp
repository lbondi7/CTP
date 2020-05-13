#include "VkManager.h"
#include "Locator.h"
#include "Devices.h"

#include <stdexcept>


VkManager::~VkManager()
{
}

void VkManager::Init()
{

	//device = Locator::GetDevices()->GetDevice();

}


VkFence VkManager::GetAvailableFence()
{
	//for (auto& f : fences)
	//{
	//	if (vkGetFenceStatus(device, f) == VK_SUCCESS)
	//	{
	//		vkResetFences(device, 1, &f);
	//		return f;
	//	}
	//}

	//fences.resize(static_cast<int>(fences.size()) + 1);

	//VkFenceCreateInfo fenceInfo = {};
	//fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
	//fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

	//if(vkCreateFence(device, &fenceInfo, nullptr, &fences[static_cast<int>(fences.size()) - 1]) != VK_SUCCESS)
	//{ 
	//	throw std::runtime_error("VkManager: failed to create fence!");
	//}

	//vkResetFences(device, 1, &fences[static_cast<int>(fences.size()) - 1]);
	//return fences[static_cast<int>(fences.size()) - 1];
	return nullptr;
}
