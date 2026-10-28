#pragma once
#include <raylib.h>
#include <raymath.h>
#include <iostream>

// Base particle class
// Other classes will handle the shapes
class Particle
{
public:
	Particle(Model* t_model);

	void Draw();
	void Update();

	void Spawn(Vector3 t_pos, Vector3 t_dir, float t_speed, float t_scale, Color t_col = RED);
	bool IsActive() const { return active; }

	void SetSpeeds(float t_min, float t_max);

	// Optionals
	void SetGravity(float t_gravityForce);
	void SetDrag(float t_dragAmount);

private:
	bool active = false;
	Model* model = nullptr;

	Vector3 position;
	Vector3 velocity;

	float speed = 0.0f;
	Vector3 scale = { 1.0f, 1.0f, 1.0f };
	const float MIN_SCALE = 0.01f;

	float rotation = 0.0f;
	float rotationSpeed = 1.0f;

	bool gravityOn = false;
	float gravityForce = 0.0f;

	bool dragOn = false;
	float dragAmount = 0.99f;

	Color color;
};

