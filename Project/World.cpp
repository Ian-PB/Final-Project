#include "World.h"

void World::Init()
{
    SetupNoise();

    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int z = 0; z < DEPTH; z++)
            {
                Vector3 newPos = { chunks[x][y][z].GetChunkSize() * x, chunks[x][y][z].GetChunkSize() * y, chunks[x][y][z].GetChunkSize() * z };
                chunks[x][y][z].SetPosition(newPos);
                chunks[x][y][z].Init(&noise);
            }
        }
    }
}

void World::Draw()
{
    for (int x = 0; x < WIDTH; x++)
        for (int y = 0; y < HEIGHT; y++)
            for (int z = 0; z < DEPTH; z++)
            {
                chunks[x][y][z].Draw();
            }
}

void World::Update()
{
    KeyboardInputs();

    for (int x = 0; x < WIDTH; x++)
    {
        for (int y = 0; y < HEIGHT; y++)
        {
            for (int z = 0; z < DEPTH; z++)
            {
                chunks[x][y][z].Update();
            }
        }
    }

    // Rebuild all chunks
    if (completeRebuild)
    {
        for (int x = 0; x < WIDTH; x++)
        {
            for (int y = 0; y < HEIGHT; y++)
            {
                for (int z = 0; z < DEPTH; z++)
                {
                    chunks[x][y][z].UpdateDensities();
                    chunks[x][y][z].GenerateMesh(surfaceLevel);

                    completeRebuild = false;
                }
            }
        }
    }
}

void World::KeyboardInputs()
{
    if (IsKeyReleased(KEY_G))
    {
        noiseSeed = (int)(rand() % 9999);
        noise.SetSeed(noiseSeed);
        completeRebuild = true;
    }

    if (IsKeyDown(KEY_UP))
    {
        surfaceLevel += 0.01f;

        if (surfaceLevel > 1.0f)
            surfaceLevel = 1.0f;

        completeRebuild = true;
    }
    else if (IsKeyDown(KEY_DOWN))
    {
        surfaceLevel -= 0.01f;

        if (surfaceLevel < -1.0f)
            surfaceLevel = -1.0f;

        completeRebuild = true;
    }
}

void World::SetupNoise()
{
    noiseSeed = (int)(rand() % 9999);
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(frequency);
    noise.SetSeed(noiseSeed);
}