#pragma once
#include<raylib.h>
#include "FastNoiseLite.h"

class BaseNoise
{
public:
	BaseNoise(Vector3 t_worldDimensions);
	virtual void Init() {}

	void SetSeed(int t_seed);
	void SetFrequency(float t_frequency);

	virtual float GetNoise(Vector3 t_pos);

protected:
	int seed = 0;
	float frequency = 0.1f;
	FastNoiseLite noise;

	Vector3 worldSize;

	static const float SOLID_VALUE;
	static const float EMPTY_VALUE;
};

