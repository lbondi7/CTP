#pragma once

#include "VkConstants.h"

#include <vector>
//#include <deque>

class VkManager
{
public:
	VkManager() = default;
	~VkManager();

	void Init();


	VkFence GetAvailableFence();

	//VkFence GetAvailableFence();


private:

	VkDevice device;


	std::vector<VkFence> fences;
};

