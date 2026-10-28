#pragma once
#include <raylib.h>
#include <raymath.h>
#include <iostream>

// Base particle class
// Other classes will handle the shapes
class Particle
{
public:
	Particle();

	void Draw();
	void Update();

	void Spawn(Vector3 t_pos, Vector3 t_dir, float t_speed, Color t_col = RED);

	void SetSpeeds(float t_min, float t_max);

	// Optionals
	void SetGravity(float t_gravityForce);
	void SetDrag(float t_dragAmount);

private:
	bool active = false;

	Vector3 position;
	Vector3 velocity;

	bool gravityOn = false;
	float gravityForce = 0.0f;

	bool dragOn = false;
	float dragAmount = 0.99f;

	Color color;
};

