#include "BaseNoise.h"

const float BaseNoise::SOLID_VALUE = -1.0f;
const float BaseNoise::EMPTY_VALUE = 1.0f;

BaseNoise::BaseNoise(Vector3 t_worldDimensions) : worldSize(t_worldDimensions)
{
    noise.SetNoiseType(FastNoiseLite::NoiseType_Perlin);
    noise.SetFrequency(frequency);
    noise.SetSeed(seed);
}

void BaseNoise::SetSeed(int t_seed)
{
    seed = t_seed; 
    noise.SetSeed(seed);
}

void BaseNoise::SetFrequency(float t_frequency)
{
    frequency = t_frequency;
    noise.SetFrequency(frequency);
}

float BaseNoise::GetNoise(Vector3 t_pos)
{
    float val = noise.GetNoise(t_pos.x, t_pos.y, t_pos.z);

    return val;
}
