#include "raylib.h"
#include "game.h"

void Game::Init()
{
    world.Init();
    player.Init();
}

void Game::Draw()
{
    DrawFPS(0, 0);

    BeginMode3D(player.GetCameraRef());

        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f }, LIGHTGRAY); // Draw ground

        world.Draw();

    EndMode3D();
}

void Game::Update()
{
    world.Update();

    player.Update();
}
