#pragma once
#include "BaseBuildingBlock.h"
class SphereBuildingBlock : public BaseBuildingBlock
{
public:
	SphereBuildingBlock(const Camera& t_cam);
	virtual void InitModel() override;

	virtual std::shared_ptr<BaseBuildingBlock> CreateClone() override;
private:
	virtual void UpdateBounds() override;
	virtual float SignedDist(Vector3 t_pointPos) override;

private:
	float baseRadius = 0.6f;
};

