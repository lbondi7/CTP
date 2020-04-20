#include "DebugPrinter.h"

#include <iostream>

std::mutex DebugPrinter::printQueueMutex;

void DebugPrinter::Print(const std::string& strToPrint)
{
	std::lock_guard<std::mutex> lock(printQueueMutex);
	std::cout << strToPrint << std::endl;
}

void DebugPrinter::Print(const glm::vec3& v)
{
	std::lock_guard<std::mutex> lock(printQueueMutex);
	auto vec = std::to_string(v.x) + ", " + std::to_string(v.y) + ", " + std::to_string(v.z);

	std::cout << vec << std::endl;
}
