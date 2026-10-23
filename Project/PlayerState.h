#pragma once
#include <raylib.h>

class Player;

class PlayerState
{
public:
	PlayerState(Player& t_player) : p(t_player) 
	{ 
	}
	virtual ~PlayerState() = default;

	virtual void Update() = 0;

	virtual void OnLeaveState() {}
	virtual void OnEnterState() {}

protected:
	Player& p;
};

#include "Player.h"