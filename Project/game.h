#pragma once
#include "World.h"
#include "Player.h"
#include "CylinderCollider.h"

class Game
{
public:
    void Init();
    void Draw();
    void Update();

private:
    std::shared_ptr<Player> player;
    CylinderCollider collider;

    World world;
};