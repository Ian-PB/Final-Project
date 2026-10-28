#include "ParticleSpawner.h"

ParticleSpawner::ParticleSpawner()
{
	for (int i = 0; i < 20; i++)
		particles.push_back(std::make_shared<Particle>());
}

void ParticleSpawner::Spawn(Vector3 t_pos)
{
	for (const std::shared_ptr<Particle> p : particles)
	{
		float dirAngleXY = (rand() % 360) * DEG2RAD;
		float dirAngleZ = (rand() % 360) * DEG2RAD;
		Vector3 dir = { cosf(dirAngleZ) * cosf(dirAngleXY), sinf(dirAngleZ), cosf(dirAngleZ) * sinf(dirAngleXY) };

		float speed = ((rand() % (int)(maxSpeed * 10)) + (minSpeed * 10)) / 10.0f;

		int randColor = rand() % colors.size();
		Color color = colors[randColor];

		p->Spawn(t_pos, dir, speed, color);
	}
}

void ParticleSpawner::Draw()
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->Draw();
	}
}

void ParticleSpawner::Update()
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->Update();
	}
}

void ParticleSpawner::SetSpeeds(float t_min, float t_max)
{
	minSpeed = t_min;
	maxSpeed = t_max;
}

void ParticleSpawner::AddColor(Color t_color)
{
	colors.push_back(t_color);
}

void ParticleSpawner::SetGravity(float t_gravityForce)
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->SetGravity(t_gravityForce);
	}
}

void ParticleSpawner::SetDrag(float t_dragAmount)
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->SetDrag(t_dragAmount);
	}
}
