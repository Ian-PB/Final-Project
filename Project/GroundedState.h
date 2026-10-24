#pragma once
#include "PlayerState.h"

class GroundedState : public PlayerState
{
public:
	GroundedState(Player& t_player);
	virtual void Update() override;
private:
	bool OnWalkableSlope();
};