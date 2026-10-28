#include "raylib.h"
#include "game.h"

void Game::Init()
{
    Lighting::Init();
    world.Init();

    player = std::make_shared<Player>(world);
    player->Init();

    world.InitColliders();
    MeshCollider::AddTrackedPos(player->GetPosRef());

    pauseMenu = std::make_shared<PauseMenu>(*player, world);
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

    pauseMenu->Draw();
}

void Game::Update()
{
    if (IsKeyPressed(KEY_ESCAPE))
        pauseMenu->ToggleActive();

    // Don't update the rest if paused
    if (pauseMenu->IsActive())
        return;

    world.Update();

    player->Update();
}
