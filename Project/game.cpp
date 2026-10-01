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

    testChunk.Init();
}

void Game::Draw()
{
    DrawFPS(0, 0);

    BeginMode3D(camera);

        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 32.0f, 32.0f }, LIGHTGRAY); // Draw ground

        // Draw triangle
        DrawModel(model, { 0.0f, 3.0f, 0.0f }, 1.0f, BLUE);
        DrawModelWires(model, { 0.0f, 3.0f, 0.0f }, 1.0f, RED);

        testChunk.Draw();

    EndMode3D();
}

void Game::Update()
{
    testChunk.Update();
    UpdateCamera(&camera, CAMERA_FIRST_PERSON);
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
