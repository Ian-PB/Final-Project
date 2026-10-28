#pragma once
#include <memory>
#include <vector>
#include "Particle.h"

class ParticleSpawner
{
public:
	ParticleSpawner(Model* t_particleModel, int t_amount, int t_MAX_AMOUNT);

	void Spawn(Vector3 t_pos);
	void Draw();
	void Update();

	void SetScales(float t_min, float t_max);
	void SetSpeeds(float t_min, float t_max);
	void AddColor(Color t_color);

	// Optionals
	void SetGravity(float t_gravityForce);
	void SetDrag(float t_dragAmount);

private:
	void SpawnSingle(Vector3 t_pos);

	Model* particleModel = nullptr;

	// Holds any type of particle. Will be taken from a pool
	std::vector<std::shared_ptr<Particle>> particles;
	int MAX_AMOUNT = 0;
	int amount = 0;

	float minSpeed = 0.0f;
	float maxSpeed = 0.0f;

	float minScale = 0.0f;
	float maxScale = 0.0f;

	std::vector<Color> colors;
};

