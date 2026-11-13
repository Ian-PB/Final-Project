#include "BaseNoise.h"

BaseNoise::BaseNoise()
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
    frequency = 0.1f; 
    noise.SetFrequency(frequency);
}

float BaseNoise::GetNoise(Vector3 t_pos)
{
    float val = noise.GetNoise(t_pos.x, t_pos.y, t_pos.z);

    return val;
}
