#pragma once
#include "PlayerState.h"

class AerialState : public PlayerState
{
public:
	AerialState(Player& t_player);
	virtual void Update() override;

	virtual void OnEnterState() override;
private:
	bool HitGroundTimer();
	bool hitGround = false;
	const float TIME_TILL_STATE_CHANGE = 0.2f;
	float timer = 0.0f;
};

