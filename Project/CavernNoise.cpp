#include "CavernNoise.h"
#include <raymath.h>
#include <cmath>
#include <algorithm>

CavernNoise::CavernNoise(Vector3 t_worldSize) : BaseNoise(t_worldSize)
{
}

void CavernNoise::Init()
{
    // Give new values for original noise
    SetFrequency(0.05f);

    // Setup Rough Noise for the details
    roughNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    roughNoise.SetSeed(seed + 1);
    roughNoise.SetFrequency(0.2f);
}

float CavernNoise::GetNoise(Vector3 t_pos)
{
    float baseNoise = BaseNoise::GetNoise(t_pos);
    float roughness = roughNoise.GetNoise(t_pos.x, t_pos.y, t_pos.z);

    // Normalize position
    Vector3 normalizedCoords = { Clamp(t_pos.x / worldSize.x, 0.0f, 1.0f), Clamp(t_pos.y / worldSize.y, 0.0f, 1.0f), Clamp(t_pos.z / worldSize.z, 0.0f, 1.0f) };

    // Get distance to nearest edge per axis
    float distX = std::min(normalizedCoords.x, 1.0f - normalizedCoords.x);
    float distY = std::min(normalizedCoords.y, 1.0f - normalizedCoords.y);
    float distZ = std::min(normalizedCoords.z, 1.0f - normalizedCoords.z);
    float edgeDist = std::min(distX, std::min(distY, distZ));

    float edgeThickness = 0.1f + (roughness * 0.01f);
    float edgeSignedDist = edgeDist - edgeThickness;
    float edgeDensity = edgeSignedDist * 10.0f;

    // Combine base with edges
    float result = std::min(baseNoise, edgeDensity);

    return result;
}
