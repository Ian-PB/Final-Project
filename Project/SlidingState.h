#pragma once
#include "PlayerState.h"
class SlidingState : public PlayerState
{
public:
	SlidingState(Player& t_player);
	virtual void Update() override;

	virtual void OnEnterState() override;
};