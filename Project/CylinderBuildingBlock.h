#pragma once
#include "BaseBuildingBlock.h"
class CylinderBuildingBlock : public BaseBuildingBlock
{
public:
	CylinderBuildingBlock(const Camera& t_cam);
	virtual void InitModel() override;

private:
	virtual void UpdateBounds() override;
	virtual float SignedDist(Vector3 t_pointPos) override;

private:
	float baseRadius = 0.6f;
	float baseHeight = 1.2f;
};

