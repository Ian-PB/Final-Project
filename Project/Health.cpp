#include "Health.h"
#include <raylib.h>

Health::Health(int t_owner) : NetworkObject(t_owner)
{
	ResetHealth();
}

void Health::Draw(Camera& t_cam)
{
	// Border
	// DrawBillboard(t_cam, )
}

void Health::Update()
{
}

void Health::TakeDamage(int t_amount)
{
	currentHealth -= t_amount;

	if (currentHealth <= 0)
	{
		currentHealth = 0;
		alive = false;
	}
}

std::vector<char> Health::Serialise()
{
	return std::vector<char>();
}

NetworkObject* Health::UpdateFromDeserialisation(const char* t_data)
{
	return nullptr;
}

void Health::ResetHealth()
{
	currentHealth = maxHealth;
	alive = true;
}
