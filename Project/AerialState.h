#pragma once
#include "PlayerState.h"

class AerialState : public PlayerState
{
public:
	AerialState(Player& t_player);
	virtual void Update() override;
};

