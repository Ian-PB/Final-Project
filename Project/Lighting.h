#pragma once
#include <raylib.h>
#include <vector>

struct Light
{
	bool active = true;

	Vector3 position = { 0.0f, 0.0f, 0.0f };
	Color color = WHITE;

	float strength = 10.0f;
	// More specific details can be added in the future
};

// This will be a static class that holds all lights.
// I will use this to pass it to the shaders.
class Lighting
{
public:
	static void Init();
	static Light& CreateLight(Color t_color = WHITE);

	// Send details to the shader given
	static void SendToShader(Shader& shader);
private:
	// Reference to all lights
	static std::vector<Light> lights;
	static const int MAX_LIGHTS = 50;
};

