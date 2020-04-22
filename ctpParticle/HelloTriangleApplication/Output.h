#pragma once

#include <iostream>
#include <string>
#include <fstream>
#include <deque>
#include <chrono>


struct Output {

	void Outputy()
	{
		std::string fn = "timestampsSDFGPU.txt";

		std::ofstream file;
		file.open(fn);
		if (!file.is_open())
			return;
		std::string buffer = "";
		//std::cout << std::to_string(timestamps.size());
		for (int i = 0; i < timestamps.size(); i++)
		{
			//file << "delta " << std::to_string(i) << ": " << std::to_string(timestamps[i]) << "\n";
			buffer = "delta " + std::to_string(i) + ": " + std::to_string(timestamps[i]) + "\n";

			file << buffer.c_str();
		}
		file.close();

	}

	void Update() {

		auto currentTime = std::chrono::high_resolution_clock::now();
		auto delta = std::chrono::duration<float, std::chrono::milliseconds::period>(currentTime - oldTime).count();
		oldTime = std::chrono::high_resolution_clock::now();

		timestamps.emplace_back(delta);
		if (timestamps.size() > 50000)
			timestamps.pop_front();
	}

	std::chrono::steady_clock::time_point oldTime;
	std::deque<float> timestamps;

};
