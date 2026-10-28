#include "Particle.h"

Particle::Particle(Model* t_model) : model(t_model)
{
	active = false;
}

void Particle::Draw()
{
	if (!active)
		return;

	if (model != nullptr)
	{
		DrawModelEx(*model, position, Vector3Ones, rotation, scale, WHITE);
	}
	else
		DrawSphere(position, scale.x, color);
}

void Particle::Update()
{
	if (!active)
		return;

	if (gravityOn)
		velocity.y += gravityForce;

	scale = Vector3Scale(scale, 0.99f);
	if (scale.x < MIN_SCALE)
	{
		active = false;
		return;
	}
	rotation += rotationSpeed;

	position = Vector3Add(position, velocity);

	if (dragOn)
		velocity = Vector3Scale(velocity, dragAmount);
}

void Particle::Spawn(Vector3 t_pos, Vector3 t_dir, float t_speed, float t_scale, Color t_col)
{
	active = true;
	position = t_pos;
	scale = { t_scale, t_scale, t_scale };
	rotation = rand() % 360;
	color = t_col;

	velocity = Vector3Scale(t_dir, t_speed);
}

void Particle::SetGravity(float t_gravityForce)
{
	gravityOn = true;
	gravityForce = -t_gravityForce;
}

void Particle::SetDrag(float t_dragAmount)
{
	dragOn = true;
	dragAmount = t_dragAmount;
}
