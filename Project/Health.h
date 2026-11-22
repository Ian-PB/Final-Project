#pragma once
#include <raylib.h>
#include "NetworkObject.h"

class Health : public NetworkObject
{
public:
	Health(int t_owner);

	void Draw(Camera& t_cam);
	void Update();

	void TakeDamage(int t_amount);

	virtual std::vector<char> Serialise() override;
	virtual NetworkObject* UpdateFromDeserialisation(const char* t_data) override;

	void ResetHealth();
	void SetHealth(int t_health) { currentHealth = t_health; }
private:

	bool alive = true;
	int currentHealth = 0;
	int maxHealth = 100;

	// Visuals
	Rectangle borderRect;
	Rectangle fillRect;
};

