#pragma once

#include <glm/glm.hpp>
#include <string>
#include <mutex>

struct DebugPrinter
{
public:
	static void Print(const std::string& strToPrint);

	static void Print(const glm::vec3& v);

private:
	static std::mutex printQueueMutex;
};