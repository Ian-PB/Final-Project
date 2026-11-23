#pragma once
#include <raylib.h>
#include "NetworkObject.h"

class Health : public NetworkObject
{
public:
	Health(int t_owner);
	~Health();

	void DrawUI();
	void DrawOthers(Camera& t_cam);
	void Update();

	void TakeDamage(int t_amount);

	virtual std::vector<char> Serialise() override;
	virtual NetworkObject* UpdateFromDeserialisation(const char* t_data) override;

	void ResetHealth();
	void SetHealth(int t_health) { currentHealth = t_health; }

	void SetPosition(Vector3 t_pos) { position = t_pos; }

private:

	bool alive = true;
	int currentHealth = 0;
	int maxHealth = 100;

	Vector3 position;

	// Visuals
	Rectangle borderRect;
	Rectangle fillRect;
	float maxFill = 0.0f;
	// 3D
	RenderTexture2D healthBarTexture;
	Rectangle healthBarSource;
};

