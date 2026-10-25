#pragma once
#include "World.h"
#include "Player.h"
#include "PauseMenu.h"

class Game
{
public:
    void Init();
    void Draw();
    void Update();

private:
    std::shared_ptr<Player> player;

    World world;

    std::shared_ptr<PauseMenu> pauseMenu;
};