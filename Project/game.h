#pragma once
#include "World.h"
#include "Player.h"

class Game
{
public:
    void Init();
    void Draw();
    void Update();

private:
    Player player;

    World world;
};