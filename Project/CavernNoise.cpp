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
    SetFrequency(0.04f);

    // Setup Rough Noise for the details
    roughNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    roughNoise.SetSeed(seed + 1);
    roughNoise.SetFrequency(0.2f);

    detailNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    detailNoise.SetSeed(seed + 2);
    detailNoise.SetFrequency(0.2f);
}

float CavernNoise::GetNoise(Vector3 t_pos)
{
    float baseNoise = BaseNoise::GetNoise(t_pos);

    // Roughness
    float roughNoiseVal = roughNoise.GetNoise(t_pos.x, t_pos.y, t_pos.z);
    const float TERRAIN_ROUGHNESS = roughNoiseVal * 0.05f;
    const float EDGE_ROUGHNESS = roughNoiseVal * 0.008f;

    // Details
    float detailNoiseVal = detailNoise.GetNoise(t_pos.x, t_pos.y, t_pos.z);
    const float DETAILS = detailNoiseVal * 0.05f;

    // Normalize position
    Vector3 normalizedCoords = { Clamp(t_pos.x / worldSize.x, 0.0f, 1.0f), Clamp(t_pos.y / worldSize.y, 0.0f, 1.0f), Clamp(t_pos.z / worldSize.z, 0.0f, 1.0f) };

    // Get distance to nearest edge per axis
    float distX = std::min(normalizedCoords.x, 1.0f - normalizedCoords.x);
    float distY = std::min(normalizedCoords.y, 1.0f - normalizedCoords.y);
    float distZ = std::min(normalizedCoords.z, 1.0f - normalizedCoords.z);
    float edgeDist = std::min(distX, std::min(distY, distZ));

    float edgeThickness = EDGE_MIN_THICKNESS + EDGE_ROUGHNESS;
    float edgeSignedDist = edgeDist - edgeThickness;
    float edgeDensity = edgeSignedDist * 10.0f;

    float baseRoughened = baseNoise + TERRAIN_ROUGHNESS + DETAILS;

    // Combine base with edges
    float result = std::min(baseRoughened, edgeDensity);

    return result;
}
