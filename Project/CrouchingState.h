#pragma once
#include "PlayerState.h"

class CrouchingState : public PlayerState
{
public:
	CrouchingState(Player& t_player);
	virtual void Update() override;

	virtual void OnEnterState() override;
};

