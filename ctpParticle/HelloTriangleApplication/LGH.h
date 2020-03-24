#pragma once

#include "Constants.h"
#include "Bounds.h"

#include <vector>

class LGH
{
public:
	LGH() = default;
	~LGH();

	void Create(const std::vector<Light>& pl);

	void Recreate(const std::vector<Light>& pl);


	const std::vector<Light>& Lights() { return lights; }

	const Light& Lights(int i) { return lights[i]; }

private:

	std::vector<Light> lights;
	std::vector<Bounds> bounds;
	Bounds bound;

	//std::vector<std::vector<std::vector<LightShits>>> lights;

};

