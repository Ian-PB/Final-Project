#include "raylib.h"
#include "game.h"

void Game::Init()
{
    SetupCamera();

    mesh.triangleCount = 1;
    mesh.vertexCount = 1 * 3;
    mesh.vertices = new float[mesh.vertexCount * 3];
    mesh.indices = new unsigned short[mesh.vertexCount];

    for (int i = 0; i < mesh.triangleCount; ++i)
    {
        float triangle[9] = { 0.f, 0.f, i, 1.f, 0.f, i, 0.f, 1.f, i };
        for (int j = 0; j < 9; ++j)
            mesh.vertices[i * 9 + j] = triangle[j];
    }
    for (unsigned short ix = 0; ix < mesh.triangleCount * 3; ++ix)
        mesh.indices[ix] = ix;

    UploadMesh(&mesh, true);
    model = LoadModelFromMesh(mesh);

    for (int x = 0; x < 3; x++)
    {
        for (int y = 0; y < 3; y++)
        {
            for (int z = 0; z < 3; z++)
            {
                Vector3 newPos = { chunks[x][y][z].GetChunkSize() * x, chunks[x][y][z].GetChunkSize() * y, chunks[x][y][z].GetChunkSize() * z };
                chunks[x][y][z].SetPosition(newPos);
                chunks[x][y][z].Init();
            }
        }
    }
}

void Game::Draw()
{
    DrawFPS(0, 0);

    BeginMode3D(camera);

        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f }, LIGHTGRAY); // Draw ground

        // Draw triangle
        DrawModel(model, { 0.0f, 3.0f, 0.0f }, 1.0f, BLUE);
        DrawModelWires(model, { 0.0f, 3.0f, 0.0f }, 1.0f, RED);

        for (int x = 0; x < 3; x++)
            for (int y = 0; y < 3; y++)
                for (int z = 0; z < 3; z++)
                {
                    chunks[x][y][z].Draw();
                }

    EndMode3D();
}

void Game::Update()
{
    for (int x = 0; x < 3; x++)
        for (int y = 0; y < 3; y++)
            for (int z = 0; z < 3; z++)
            {
                chunks[x][y][z].Update();
            }

    UpdateCamera(&camera, CAMERA_FIRST_PERSON);

    if (IsKeyDown(KEY_SPACE))
    {
        camera.position.y += 0.2f;
    }
    else if (IsKeyDown(KEY_LEFT_SHIFT))
    {
        camera.position.y -= 0.2f;
    }
}

void Game::SetupCamera()
{
    camera.position = { 0.0f, 2.0f, 4.0f };             // Camera position
    camera.target = { 0.0f, 2.0f, 0.0f };               // Camera looking at point
    camera.up = { 0.0f, 1.0f, 0.0f };                   // Camera up vector (rotation towards target)
    camera.fovy = 60.0f;                                // Camera field-of-view Y
    camera.projection = CAMERA_PERSPECTIVE;             // Camera projection type

    DisableCursor();
}
