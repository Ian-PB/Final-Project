#include "ParticleSpawner.h"

ParticleSpawner::ParticleSpawner(Model* t_particleModel, int t_amount, int t_MAX_AMOUNT) : particleModel(t_particleModel), amount(t_amount), MAX_AMOUNT(t_MAX_AMOUNT)
{
	for (int i = 0; i < MAX_AMOUNT; i++)
		particles.push_back(std::make_shared<Particle>(particleModel));
}

void ParticleSpawner::SetModel(Model* t_particleModel)
{
	particleModel = t_particleModel;
	for (std::shared_ptr<Particle> p : particles)
		p->SetModel(particleModel);
}

void ParticleSpawner::Spawn(Vector3 t_pos)
{
	for (int i = 0; i < amount; i++)
	{
		// Find an inactive particle and spawn it
		SpawnSingle(t_pos);
	}
}

void ParticleSpawner::Draw()
{
	for (const std::shared_ptr<Particle> p : particles)
	{
		p->Draw();
	}
}

void ParticleSpawner::Update()
{
	for (const std::shared_ptr<Particle> p : particles)
	{
		p->Update();
	}
}

void ParticleSpawner::SetScales(float t_min, float t_max)
{
	minScale = t_min;
	maxScale = t_max;
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
	for (const std::shared_ptr<Particle> p : particles)
	{
		p->SetGravity(t_gravityForce);
	}
}

void ParticleSpawner::SetDrag(float t_dragAmount)
{
	for (int i = 0; i < particles.size(); i++)
	{
		particles[i]->SetDrag(t_dragAmount);
	}
}

void ParticleSpawner::SpawnSingle(Vector3 t_pos)
{
	for (const std::shared_ptr<Particle> p : particles)
	{
		if (p->IsActive())
			continue;

		float dirAngleXY = (rand() % 360) * DEG2RAD;
		float dirAngleZ = (rand() % 360) * DEG2RAD;
		Vector3 dir = { cosf(dirAngleZ) * cosf(dirAngleXY), sinf(dirAngleZ), cosf(dirAngleZ) * sinf(dirAngleXY) };

		float speed = ((rand() % (int)(maxSpeed * 10)) + (minSpeed * 10)) / 10.0f;
		float scale = ((rand() % (int)(maxScale * 10)) + (minScale * 10)) / 10.0f;

		int randColor = rand() % colors.size();
		Color color = colors[randColor];

		p->Spawn(t_pos, dir, speed, scale, color);
		return;
	}
}
