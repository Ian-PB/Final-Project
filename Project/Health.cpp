#include "Health.h"
#include <raylib.h>
#include <raymath.h>
#include <cstdint>
#include <string.h>

Health::Health(int t_owner) : NetworkObject(t_owner)
{
	ResetHealth();

	// Border
	borderRect.width = 200.0f;
	borderRect.height = 25.0f;
	//borderRect.x = borderRect.width / 2.0f;
	//borderRect.y = GetScreenHeight() * 0.9f;
	borderRect.x = 0.0f;
	borderRect.y = 0.0f;

	// Fill
	maxFill = borderRect.width * 0.98f;
	fillRect.width = maxFill;
	fillRect.height = borderRect.height * 0.8f;
	fillRect.x = borderRect.x + borderRect.width * 0.01f;
	fillRect.y = borderRect.y + borderRect.height * 0.1f;

	healthBarTexture = LoadRenderTexture(borderRect.width, borderRect.height);
	healthBarSource = { 0.0f, 0.0f, (float)healthBarTexture.texture.width, -(float)healthBarTexture.texture.height };
}

Health::~Health()
{
	UnloadRenderTexture(healthBarTexture);
}

void Health::DrawTexture()
{
	BeginTextureMode(healthBarTexture);

		DrawRectangleRounded(borderRect, 0.5f, 5, RAYWHITE);

		fillRect.width = maxFill * ((float)currentHealth / maxHealth);
		DrawRectangleRounded(fillRect, 0.5f, 5, GREEN);

	EndTextureMode();
}

void Health::DrawUI()
{
	Vector2 pos;
	pos.x = (GetScreenWidth() * 0.5f) - (healthBarTexture.texture.width * 0.5f);
	pos.y = (GetScreenHeight() * 0.9f) + (healthBarTexture.texture.height * 0.5f);

	DrawTextureEx(healthBarTexture.texture, pos, 0, 1, WHITE);
}

void Health::DrawOthers(const Camera& t_cam, Vector3 t_pos)
{
	position = t_pos;
	position.y += yOffset;
	DrawBillboardRec(t_cam, healthBarTexture.texture, healthBarSource, position, { 4.0f, 0.5f }, WHITE);
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
	std::vector<char> buffer;

	auto write = [&](const void* data, size_t size) {
		const char* bytes = reinterpret_cast<const char*>(data);
		buffer.insert(buffer.end(), bytes, bytes + size);
		};

	SerialiseBase(buffer);

	write(&currentHealth, sizeof(currentHealth));
	write(&maxHealth, sizeof(maxHealth));

	// Set true length of buffer
	uint32_t len = static_cast<uint32_t>(buffer.size());
	memcpy(buffer.data(), &len, sizeof(len));

	return buffer;
}

NetworkObject* Health::UpdateFromDeserialisation(const char* t_data)
{
	size_t offset = 0;
	auto read = [&](void* out, size_t s) {
		memcpy(out, t_data + offset, s);
		offset += s;
		};

	read(&currentHealth, sizeof(currentHealth));
	read(&maxHealth, sizeof(maxHealth));

	return this;
}

void Health::ResetHealth()
{
	currentHealth = maxHealth;
	alive = true;
}
