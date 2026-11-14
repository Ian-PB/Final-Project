#pragma once
#include "BaseNoise.h"

class CavernNoise : public BaseNoise
{
public:
	CavernNoise(Vector3 t_worldDimensions);
	virtual void Init() override;

	virtual void SetSeed(int t_seed) override;
	virtual float GetNoise(Vector3 t_pos) override;

private:
	FastNoiseLite floorNoise;

	FastNoiseLite roughNoise;
	FastNoiseLite detailNoise;

	const float EDGE_MIN_THICKNESS = 0.1f;
};

