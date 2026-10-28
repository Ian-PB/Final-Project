#pragma once
#include <memory>
#include <vector>
#include "Particle.h"

class ParticleSpawner
{
public:
	ParticleSpawner();
	void SetAmount(int t_amount) { amount = t_amount; }

	void Spawn(Vector3 t_pos);
	void Draw();
	void Update();

	void SetSpeeds(float t_min, float t_max);
	void AddColor(Color t_color);

	// Optionals
	void SetGravity(float t_gravityForce);
	void SetDrag(float t_dragAmount);

private:
	// Holds any type of particle. Will be taken from a pool
	std::vector<std::shared_ptr<Particle>> particles;
	int amount = 0;

	float minSpeed = 0.0f;
	float maxSpeed = 0.0f;

	std::vector<Color> colors;
};

