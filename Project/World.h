#pragma once
#include "Chunk.h"
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class World
{
// Needs to be before the getChunks function
private:
	static const int WIDTH = 5;
	static const int HEIGHT = 5;
	static const int DEPTH = 5;

public:
	World();
	~World();

	void Init();
	void Draw();
	void Update();

	Vector3 GetDimensions() const { return { WIDTH, HEIGHT, DEPTH }; }
	const Chunk& GetChunk(int x, int y, int z) const { return chunks[x][y][z]; }
	Chunk& GetChunkFromWorldPos(Vector3 t_pos);
	int GetChunkIndexFromWorldPos(Vector3 t_pos);
	int GetFlatIndex(int x, int y, int z) { return (x * (HEIGHT * DEPTH) + y * DEPTH + z); }

	// World editting
	void EditSphere(Vector3 t_pos, float t_radius, bool destroy = true);

private:
	void UpdateAllChunks();

	// World Dimensions
	int totalChunks = 0;
	Chunk chunks[WIDTH][HEIGHT][DEPTH];

	float surfaceLevel = 0.0f;

	bool completeRebuild = false;

	// Shader
	Shader lightingShader;

	// Noise
	void SetupNoise();
	FastNoiseLite noise;
	float frequency = 0.1f;
	int noiseSeed;

	// Multi-threading
	void WorkerLoop();
	int amountOfThreads = 0;
	std::vector<std::thread> workers;

	static const int MAX_CHUNKS_UPDATED_PER_FRAME = 9;
	std::queue<int> chunksThatNeedWork;
	std::queue<int> chunksNeedingNewModel;
	std::mutex jobMutex;					// Protects the ongoing queue
	std::mutex jobFinishedMutex;			// Protects the finished queue
	std::condition_variable jobCondition;	// Threads wait for this to continue working

	std::atomic<bool> stopWorkers = false;
	std::atomic<int> jobsRemaining = 0;

	void KeyboardInputs();
};