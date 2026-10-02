#pragma once
#include "Chunk.h"

class World
{
public:
	void Init();
	void Draw();
	void Update();

private:
	void KeyboardInputs();

	// World Dimensions
	static const int WIDTH = 3;
	static const int HEIGHT = 3;
	static const int DEPTH = 3;
	Chunk chunks[WIDTH][HEIGHT][DEPTH];

	float surfaceLevel = 0.0f;

	bool completeRebuild = false;

	// Noise
	void SetupNoise();
	FastNoiseLite noise;
	float frequency = 0.1f;
	int noiseSeed;
};