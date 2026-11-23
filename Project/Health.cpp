#include "Health.h"
#include <raylib.h>
#include <raymath.h>

Health::Health(int t_owner) : NetworkObject(t_owner)
{
	ResetHealth();

	// Border
	borderRect.width = GetScreenWidth() * 0.5f;
	borderRect.height = GetScreenHeight() * 0.05f;
	borderRect.x = borderRect.width / 2.0f;
	borderRect.y = GetScreenHeight() * 0.9f;

	// Fill
	maxFill = borderRect.width * 0.98f;
	fillRect.width = maxFill;
	fillRect.height = borderRect.height * 0.8f;
	fillRect.x = borderRect.x + borderRect.width * 0.01f;
	fillRect.y = borderRect.y + borderRect.height * 0.1f;

	healthBarTexture = LoadRenderTexture(256.0f, 32.0f);
	healthBarSource = { 0.0f, 0.0f, (float)healthBarTexture.texture.width, -(float)healthBarTexture.texture.height };
}

Health::~Health()
{
	UnloadRenderTexture(healthBarTexture);
}

void Health::DrawUI()
{
	DrawRectangleRounded(borderRect, 0.5f, 5, RAYWHITE);

	fillRect.width = maxFill * ((float)currentHealth / maxHealth);
	DrawRectangleRounded(fillRect, 0.5f, 5, GREEN);
}

void Health::DrawOthers(Camera& t_cam)
{
	// Draw texture for health bar

	DrawBillboardRec(t_cam, healthBarTexture.texture, healthBarSource, position, { 20.0f, 10.25f }, WHITE);
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
