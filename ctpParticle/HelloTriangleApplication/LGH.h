#pragma once

#include "Constants.h"
#include "Bounds.h"

#include <vector>

class LGH
{
public:
	LGH() = default;
	~LGH();

	void Create(const std::vector<LightShits>& pl);

	void Recreate(const std::vector<LightShits>& pl);


	const std::vector<LightShits>& Lights() { return lights; }

	const LightShits& Light(int i) { return lights[i]; }

private:

	std::vector<LightShits> lights;
	std::vector<Bounds> bounds;
	Bounds bound;

	//std::vector<std::vector<std::vector<LightShits>>> lights;

};

