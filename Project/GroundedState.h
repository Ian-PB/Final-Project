#pragma once
#include "PlayerState.h"

class GroundedState : public PlayerState
{
public:
	GroundedState(Player& t_player);
	virtual void Update() override;

	virtual void OnEnterState() override;
private:
	bool OnWalkableSlope();
};