#include "World.h"
#include <mutex>
#include <math.h>
#include <algorithm>
#include <raymath.h>

World::World()
{
    unsigned int hardwareThreads = std::thread::hardware_concurrency();
    totalChunks = WIDTH * HEIGHT * DEPTH;

    amountOfThreads = std::min(totalChunks, (int)(hardwareThreads > 1 ? hardwareThreads - 1 : 1));

    for (int i = 0; i < amountOfThreads; i++)
    {
        workers.emplace_back(&World::WorkerLoop, this);
    }
}

World::~World()
{
    // Stop all workers
    stopWorkers = true;
    jobCondition.notify_all();

    for (std::thread& worker : workers)
    {
        if (worker.joinable())
            worker.join();
    }
}

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
        if (jobsRemaining == 0)
        {
            // Awake all workers
            UpdateAllChunks();
            completeRebuild = false;
        }
    }

    // Protect with mutex
    {
        std::lock_guard<std::mutex> lock(jobFinishedMutex);

        // Constantly check if any updates are needed
        for (int i = 0; i < std::min(MAX_CHUNKS_UPDATED_PER_FRAME, (int)chunksNeedingNewModel.size()); i++)
        {
            int chunkI = chunksNeedingNewModel.front();
            chunksNeedingNewModel.pop();

            // Get the x, y, z for this chunk
            int x = chunkI / (HEIGHT * DEPTH);
            int remainder = chunkI % (HEIGHT * DEPTH);
            int y = remainder / DEPTH;
            int z = remainder % DEPTH;

            // Check if changed (should be)
            if (chunks[x][y][z].IsDirty())
                chunks[x][y][z].ApplyMeshDataToModel(); // Apply new mesh to model
        }
    }
}

void World::WorkerLoop()
{
    while (true)
    {
        int chunkIndex = 0;

        {
            std::unique_lock<std::mutex> lock(jobMutex);

            jobCondition.wait(lock, [this]()
                {
                    return !chunksThatNeedWork.empty() || stopWorkers;
                });

            if (stopWorkers)
                return;

            chunkIndex = chunksThatNeedWork.front();
            chunksThatNeedWork.pop();
        } // mutex UNLOCKED HERE

        int x = chunkIndex / (HEIGHT * DEPTH);
        int remainder = chunkIndex % (HEIGHT * DEPTH);
        int y = remainder / DEPTH;
        int z = remainder % DEPTH;

        // Workers can now do this simultaneously
        chunks[x][y][z].GenerateMeshData(surfaceLevel);

        // Protect the queue
        {
            std::lock_guard<std::mutex> lock(jobFinishedMutex);
            chunksNeedingNewModel.push(chunkIndex);
        }

        jobsRemaining--;
    }
}

Chunk& World::GetChunkFromWorldPos(Vector3 t_pos)
{
    float chunkSize = chunks[0][0][0].GetChunkSize();

    // Get indexes
    int x = (int)std::floor(t_pos.x / chunkSize);
    int y = (int)std::floor(t_pos.y / chunkSize);
    int z = (int)std::floor(t_pos.z / chunkSize);

    // Make sure the indexes are within the world
    x = std::clamp(x, 0, WIDTH - 1);
    y = std::clamp(y, 0, HEIGHT - 1);
    z = std::clamp(z, 0, DEPTH - 1);

    return chunks[x][y][z];
}

int World::GetChunkIndexFromWorldPos(Vector3 t_pos)
{
    float chunkSize = chunks[0][0][0].GetChunkSize();

    // Get indexes
    int x = (int)std::floor(t_pos.x / chunkSize);
    int y = (int)std::floor(t_pos.y / chunkSize);
    int z = (int)std::floor(t_pos.z / chunkSize);

    // Make sure the indexes are within the world
    x = std::clamp(x, 0, WIDTH - 1);
    y = std::clamp(y, 0, HEIGHT - 1);
    z = std::clamp(z, 0, DEPTH - 1);

    return GetFlatIndex(x, y, z);
}

void World::EditSphere(Vector3 t_pos, float t_radius, bool destroy)
{
    float chunkSize = chunks[0][0][0].GetChunkSize();
    
    int minX = (int)std::floor((t_pos.x - t_radius) / chunkSize);
    int minY = (int)std::floor((t_pos.y - t_radius) / chunkSize);
    int minZ = (int)std::floor((t_pos.z - t_radius) / chunkSize);

    int maxX = (int)std::floor((t_pos.x + t_radius) / chunkSize);
    int maxY = (int)std::floor((t_pos.y + t_radius) / chunkSize);
    int maxZ = (int)std::floor((t_pos.z + t_radius) / chunkSize);

    // Keep within the world borders
    minX = std::clamp(minX, 0, WIDTH - 1);
    minY = std::clamp(minY, 0, HEIGHT - 1);
    minZ = std::clamp(minZ, 0, DEPTH - 1);

    maxX = std::clamp(maxX, 0, WIDTH - 1);
    maxY = std::clamp(maxY, 0, HEIGHT - 1);
    maxZ = std::clamp(maxZ, 0, DEPTH - 1);

    // Loop through the possible positions and update the chunks the sphere is withn
    for (int x = minX; x <= maxX; x++)
    {
        for (int y = minY; y <= maxY; y++)
        {
            for (int z = minZ; z <= maxZ; z++)
            {
                Chunk& chunkToChange = chunks[x][y][z];;
                // If the chunk is being changed currently don't do anything
                if (chunkToChange.IsDirty() )
                    continue;

                chunkToChange.EditSphere(t_pos, t_radius, destroy);

                int chunkToChangeIndex = GetFlatIndex(x, y, z);

                { // Protect variables in multi-threading
                    std::lock_guard<std::mutex> lock(jobMutex);
                    chunksThatNeedWork.push(chunkToChangeIndex);
                    jobsRemaining++;
                }
            }
        }
    }

    jobCondition.notify_one();
}

// Allows for splitting the work to different threads
void World::UpdateAllChunks()
{
    if (jobsRemaining > 0)
        return;

    std::lock_guard<std::mutex> lock(jobMutex);

    for (int i = 0; i < totalChunks; i++)
    {
        chunksThatNeedWork.push(i);
    }

    // Clear queue since new chunk data will be made
    {
        std::lock_guard<std::mutex> lock(jobFinishedMutex);
        while (!chunksNeedingNewModel.empty())
            chunksNeedingNewModel.pop();
    }

    jobsRemaining = totalChunks;
    jobCondition.notify_all(); // Tell workers to work
}

void World::KeyboardInputs()
{
    if (IsKeyReleased(KEY_G))
    {
        noiseSeed = (int)(rand() % 9999);
        noise.SetSeed(noiseSeed);
        completeRebuild = true;

        // Reset densities
        for (int x = 0; x < WIDTH; x++)
        {
            for (int y = 0; y < HEIGHT; y++)
            {
                for (int z = 0; z < DEPTH; z++)
                {
                    chunks[x][y][z].UpdateDensities();
                }
            }
        }
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