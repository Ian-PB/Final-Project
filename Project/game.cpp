#include "game.h"

void Game::Init()
{
    Lighting::Init();
    world.Init();

    player = std::make_shared<Player>(world);
    player->Init();

    world.InitColliders();
    MeshCollider::AddTrackedPos(player->GetPosRef());

    Vector3 worldCenter = Vector3Scale(world.GetSizeDimensions(), 0.5f);
    player->SetPos(worldCenter);

    pauseMenu = std::make_shared<PauseMenu>(*player, world);

    EnableCursor();
}

void Game::Draw()
{
    switch (gameState)
    {
    case GameStates::ChooseNetworkType:
        DrawPreGame();
        break;
    case GameStates::JoinGame:
        DrawJoinGame();
        break;
    case GameStates::Game:
        DrawGame();
        break;
    default:
        break;
    }
}

void Game::DrawPreGame()
{
    bool optionPicked = false;
    Rectangle rect;
    rect.width = GetScreenWidth();
    rect.height = GetScreenHeight() / 2.0f;
    rect.x = 0.0f;
    rect.y = 0.0f;
    if (GuiButton(rect, "Host"))
    {
        networkType = NetworkingTypes::Host;
        server.Init();
        optionPicked = true;
    }

    rect.y += GetScreenHeight() / 2.0f;
    if (GuiButton(rect, "Join"))
    {
        networkType = NetworkingTypes::Client;
        client.Init();
        optionPicked = true;
    }

    if (optionPicked)
    {
        gameState = GameStates::Game;
        DisableCursor();
    }
}

void Game::DrawJoinGame()
{
}

void Game::DrawGame()
{
    BeginMode3D(player->GetCameraRef());

        world.Draw();
        player->Draw();

    EndMode3D();

    player->Draw2D();
    DrawFPS(0, 0);

    pauseMenu->Draw();

    if (networkType == NetworkingTypes::Host)
        DrawText(server.GetServerIp().c_str(), 0, GetScreenHeight() - 20, 20, RED);
}

void Game::Update()
{
    if (gameState != GameStates::Game)
        return;

    switch (networkType)
    {
    case NetworkingTypes::Host:
        server.Update();
        break;
    case NetworkingTypes::Client:
        client.Update();
        break;
    default:
        break;
    }

    if (IsKeyPressed(KEY_ESCAPE))
        pauseMenu->ToggleActive();

    // Don't update the rest if paused
    if (pauseMenu->IsActive())
        return;

    world.Update();

    player->Update();
}
