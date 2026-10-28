#include "Particle.h"

Particle::Particle()
{
	active = false;
}

void Particle::Draw()
{
	if (!active)
		return;

	DrawSphere(position, 1.0f, color);
	std::cout << position.x << ", " << position.y << ", " << position.z << std::endl;
}

void Particle::Update()
{
	if (!active)
		return;

	if (gravityOn)
		velocity.y += gravityForce;

	position = Vector3Add(position, velocity);

	if (dragOn)
		velocity = Vector3Scale(velocity, dragAmount);
}

void Particle::Spawn(Vector3 t_pos, Vector3 t_dir, float t_speed, Color t_col)
{
	color = t_col;
	active = true;
	position = t_pos;
	velocity = Vector3Scale(t_dir, t_speed);
}

void Particle::SetGravity(float t_gravityForce)
{
	gravityOn = true;
	gravityForce = t_gravityForce;
}

void Particle::SetDrag(float t_dragAmount)
{
	dragOn = true;
	dragAmount = t_dragAmount;
}
