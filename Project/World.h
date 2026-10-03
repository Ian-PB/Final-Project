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
public:
	World();
	~World();

	void Init();
	void Draw();
	void Update();

private:
	void UpdateChunks();

	// World Dimensions
	int totalChunks = 0;
	static const int WIDTH = 5;
	static const int HEIGHT = 5;
	static const int DEPTH = 5;
	Chunk chunks[WIDTH][HEIGHT][DEPTH];

	float surfaceLevel = 0.0f;

	bool completeRebuild = false;

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