#pragma once
#include "BaseNoise.h"

class CavernNoise : public BaseNoise
{
public:
	CavernNoise(Vector3 t_worldDimensions);
	virtual void Init() override;

	virtual float GetNoise(Vector3 t_pos) override;

private:
	FastNoiseLite roughNoise;

	const float EDGE_MIN_THICKNESS = 0.1f;
};

