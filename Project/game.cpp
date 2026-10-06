#include "raylib.h"
#include "game.h"

void Game::Init()
{
    Lighting::Init();
    world.Init();

    player = std::make_shared<Player>(world);
    player->Init();

}

void Game::Draw()
{
    BeginMode3D(player->GetCameraRef());

        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f }, LIGHTGRAY); // Draw ground

        world.Draw();
        player->Draw();

    EndMode3D();

    player->Draw2D();
    DrawFPS(0, 0);
    // Crossair
    DrawCircle(GetScreenWidth() / 2.0f, GetScreenHeight() / 2.0f, 2, BLUE);
}

void Game::Update()
{
    world.Update();

    player->Update();
}
