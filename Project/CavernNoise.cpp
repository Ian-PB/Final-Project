#include "CavernNoise.h"
#include <raymath.h>
#include <cmath>
#include <algorithm>
#include <iostream>

CavernNoise::CavernNoise(Vector3 t_worldSize) : BaseNoise(t_worldSize)
{
}

void CavernNoise::Init()
{
    // Give new values for original noise
    SetFrequency(0.025f);

    // Setup Rough Noise for the larger details
    roughNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    roughNoise.SetFrequency(0.2f);

    // Setup Detail Noise for the smaller details
    detailNoise.SetNoiseType(FastNoiseLite::NoiseType_Cellular);
    detailNoise.SetFrequency(0.2f);

    // Floor height noise
    floorNoise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    floorNoise.SetFrequency(0.04f);
}

void CavernNoise::SetSeed(int t_seed)
{
    BaseNoise::SetSeed(t_seed);
    roughNoise.SetSeed(seed + 1);
    detailNoise.SetSeed(seed + 2);
    floorNoise.SetSeed(seed + 3);
}

float CavernNoise::GetNoise(Vector3 t_pos)
{
    float baseNoise = BaseNoise::GetNoise(t_pos);
    float floorHeight = floorNoise.GetNoise(t_pos.x, t_pos.z) * 50.0f;

    // Roughness
    float roughNoiseVal = roughNoise.GetNoise(t_pos.x, t_pos.y, t_pos.z);
    const float TERRAIN_ROUGHNESS = roughNoiseVal * 0.02f;
    const float EDGE_ROUGHNESS = roughNoiseVal * 0.008f;

    // Details
    float detailNoiseVal = detailNoise.GetNoise(t_pos.x, t_pos.y, t_pos.z);
    const float DETAILS = detailNoiseVal * 0.03f;

    // Normalize position
    Vector3 normalizedCoords = { Clamp(t_pos.x / worldSize.x, 0.0f, 1.0f), Clamp(t_pos.y / worldSize.y, 0.0f, 1.0f), Clamp(t_pos.z / worldSize.z, 0.0f, 1.0f) };

    // Get distance to nearest edge per axis
    float distX = std::min(normalizedCoords.x, 1.0f - normalizedCoords.x);
    float distY = std::min(normalizedCoords.y, 1.0f - normalizedCoords.y);
    float distZ = std::min(normalizedCoords.z, 1.0f - normalizedCoords.z);
    float edgeDist = std::min(distX, std::min(distY, distZ));

    float edgeDensity = 0.0f;
    float edgeThickness = EDGE_MIN_THICKNESS + EDGE_ROUGHNESS;
    // Check if it's floor
    if (t_pos.y <= floorHeight)
    {
        float floorDensity = (floorHeight - t_pos.y) / worldSize.y;
        edgeThickness += floorDensity + EDGE_ROUGHNESS;
    }

    // Get density edges
    float edgeSignedDist = edgeDist - edgeThickness;
    edgeDensity = edgeSignedDist * 10.0f;

    float centerBias = (distY * 2.0f) * 0.1f;

    float baseRoughened = (baseNoise + TERRAIN_ROUGHNESS + DETAILS) - centerBias;

    // Combine base with edges
    float result = std::min(baseRoughened, edgeDensity);

    return result;
}
