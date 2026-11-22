#include "game.h"
#include <WS2tcpip.h>
#include <Windows.h>
#include <ws2def.h>
#include <string>

void Game::Init()
{
    Lighting::Init();

    world.InitColliders();

    AddPlayer(0);

    pauseMenu = std::make_shared<PauseMenu>(*players[0], world); // player[0] is wrong currently

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
    Rectangle rect;
    rect.width = GetScreenWidth() / 2.0f;
    rect.height = GetScreenHeight() / 4.0f;
    rect.x = (GetScreenWidth() / 2.0f) - (rect.width / 2.0f);
    rect.y = (GetScreenHeight() / 2.0f) - rect.height;
    if (GuiButton(rect, "Host"))
    {
        hadIssueConnecting = false;

        networkType = NetworkingTypes::Host;
        world.Init((int)(rand() % 9999)); // Set world

        server = std::make_shared<Server>(world);
        server->Init();

        gameState = GameStates::Game;
        DisableCursor();

        players[0]->SetOwner(true);

        return;
    }

    rect.y += rect.height;
    if (GuiButton(rect, "Join"))
    {
        hadIssueConnecting = false;

        networkType = NetworkingTypes::Client;
        client = std::make_shared<Client>(world);

        gameState = GameStates::JoinGame;

        return;
    }

    // Error box
    rect.y += rect.height * 1.2f;
    if (hadIssueConnecting)
    {
        const Rectangle errorPanel = { rect.x, rect.y, rect.width, rect.height * 0.5f };
        int oldBackground = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
        int oldLine = GuiGetStyle(DEFAULT, LINE_COLOR);

        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(RED));
        GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(MAROON));

        GuiPanel(errorPanel, ERROR_CONNECTING_MSG.c_str());

        // Restore original styles
        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, oldBackground);
        GuiSetStyle(DEFAULT, LINE_COLOR, oldLine);
    }
}

void Game::DrawJoinGame()
{
    Rectangle rect;
    rect.width = GetScreenWidth() * 0.8f;
    rect.height = GetScreenHeight() * 0.8f;
    rect.x = GetScreenWidth() * 0.1f;
    rect.y = GetScreenHeight() * 0.1f;

    GuiTextInputBox(rect, "Server IP: ", "Enter the Servers IP: ", serverIpInput, rect.height * 0.9f, "Join;Cancel", &activeButton, nullptr);

    if (invalidIpInput)
    {
        Rectangle errorPanel;
        errorPanel.width = GetScreenWidth() * 0.1f;
        errorPanel.height = GetScreenHeight() * 0.1f;
        errorPanel.x = GetScreenWidth() / 2.0f - errorPanel.width / 2.0f;
        errorPanel.y = GetScreenHeight() / 2.0f - errorPanel.height / 2.0f;

        // Save panel style
        int oldBackground = GuiGetStyle(DEFAULT, BACKGROUND_COLOR);
        int oldLine = GuiGetStyle(DEFAULT, LINE_COLOR);
        // Set to error style
        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, ColorToInt(RED));
        GuiSetStyle(DEFAULT, LINE_COLOR, ColorToInt(MAROON));

        // Draw panel with error style
        GuiPanel(errorPanel, ERROR_INVALID_IP.c_str());

        // Restore original styles
        GuiSetStyle(DEFAULT, BACKGROUND_COLOR, oldBackground);
        GuiSetStyle(DEFAULT, LINE_COLOR, oldLine);
    }

    if (activeButton == 1) // Join Pressed
    {
        if (IsValidIp(serverIpInput))
        {
            invalidIpInput = false;
            client->JoinServer(serverIpInput);
            gameState = GameStates::Game;
            DisableCursor();
        }
        else
        {
            // Not a valid IP
            invalidIpInput = true;
            activeButton = -1;
            return;
        }
    }
    else if (activeButton == 2 || activeButton == 0) // Cancel
    {
        gameState = GameStates::ChooseNetworkType;
    }
}

void Game::DrawGame()
{
    BeginMode3D(camera->GetCameraRef());

        world.Draw();

        for (const std::shared_ptr<Player> player : players)
        {
            player->Draw();
        }

    EndMode3D();

    if (networkType == NetworkingTypes::Client && client->GetClientID() != -1)
    {
        {
            std::shared_ptr<Player> localPlayer = GetPlayerByOwner(client->GetClientsObjs());
            localPlayer->Draw2D();
        }
    }
    else if (networkType == NetworkingTypes::Host)
        players[0]->Draw2D();
    
    DrawFPS(0, 0);

    pauseMenu->Draw();

    if (networkType == NetworkingTypes::Host)
        DrawText(server->GetServerIp().c_str(), 0, GetScreenHeight() - 20, 20, RED);
}

void Game::Update()
{
    if (gameState != GameStates::Game)
        return;

    switch (networkType)
    {
    case NetworkingTypes::Host:
        server->Update();

        if (server->GetNewPlayersSize() > 0)
        {
            for (int owner : server->GetNewPlayers())
            {
                AddPlayer(owner);
            }

            server->ClearNewPlayers();
        }

        break;

    case NetworkingTypes::Client:
        ClientUpdate();
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

    if (networkType == NetworkingTypes::Client && client->GetClientID() != -1)
    {
        std::shared_ptr<Player> localPlayer = GetPlayerByOwner(client->GetClientsObjs());
        localPlayer->Update();
    }
    else if (networkType == NetworkingTypes::Host)
        players[0]->Update(); 
}

std::shared_ptr<Player> Game::GetPlayerByOwner(int t_owner)
{
    for (const std::shared_ptr<Player>& player : players)
    {
        if (player->GetOwner() == t_owner)
            return player;
    }

    return nullptr;
}

void Game::AddPlayer(int t_owner)
{
    // Create the player
    std::shared_ptr<Player> player = std::make_shared<Player>(t_owner, world);
    player->Init();

    // Set spawn position
    Vector3 worldCenter = Vector3Scale(world.GetSizeDimensions(), 0.5f);
    player->SetPos(worldCenter);

    // Allow mesh tracking
    MeshCollider::AddTrackedPos(player->GetPosRef());

    // Set as owner if needed
    if (networkType == NetworkingTypes::Client && client->GetClientID() != -1 && t_owner == client->GetClientsObjs())
    {
        player->SetOwner(true);
        camera = std::make_shared<MainCamera>(player->GetPosRef(), 0.8f);
        camera->SetHeight(player->GetHeight());
        player->SetCamera(camera);
    }
    else if (t_owner == 0)
    {
        camera = std::make_shared<MainCamera>(player->GetPosRef(), 0.8f);
        camera->SetHeight(player->GetHeight());
        player->SetCamera(camera);
    }

    players.push_back(player);
}

void Game::ClientUpdate()
{
    ConnectionType connection = client->JoiningTimer();
    if (connection == ConnectionType::Waiting) // Awaiting join success
    {
        int fontSize = 30;
        int textWidth = MeasureText(joiningText.c_str(), fontSize);
        DrawText(joiningText.c_str(), (GetScreenWidth() - textWidth) / 2.0f, (GetScreenHeight() - fontSize) / 2.0f, fontSize, WHITE);
    }
    else if (connection == ConnectionType::TookTooLong) // Go back to lobby
    {
        EnableCursor();
        hadIssueConnecting = true;
        gameState = GameStates::ChooseNetworkType;
    }

    client->Update();

    if (client->GetNewPlayersSize() > 0)
    {
        for (int owner : client->GetNewPlayers())
        {
            AddPlayer(owner);
        }

        client->ClearNewPlayers();
    }
}

bool Game::IsValidIp(const std::string& t_ip)
{
    IN_ADDR addr;
    return InetPtonA(AF_INET, t_ip.c_str(), &addr) == 1;
}
