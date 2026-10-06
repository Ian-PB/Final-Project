#include "Lighting.h"
#include <iostream>
#include <string>

std::vector<Light> Lighting::lights;

// Call to initialize lights vector
void Lighting::Init()
{
    lights.reserve(MAX_LIGHTS);
}

Light& Lighting::CreateLight(Color t_color)
{
    if (lights.size() >= MAX_LIGHTS)
    {
        std::cout << "MAX AMOUNT OF LIGHTS REACHED" << std::endl;
        return lights[0]; // Just return the first light
    }

    // Add a light to the vector and return the new light
    Light& newLight = lights.emplace_back();
    newLight.color = t_color;

    return newLight;
}

void Lighting::SendToShader(Shader& shader)
{
    int lightCount = std::min((int)lights.size(), MAX_LIGHTS);
    int lightCountLocation = GetShaderLocation(shader, "lightCount");
    SetShaderValue(shader, lightCountLocation, &lightCount, SHADER_UNIFORM_INT);

    for (int i = 0; i < lightCount; i++)
    {
        Light& light = lights[i];

        std::string baseLocation = "lights[" + std::to_string(i) + "]";

        // Get locations of uniforms
        int activeLoc = GetShaderLocation(shader, (baseLocation + ".lightOn").c_str());
        int positionLoc = GetShaderLocation(shader, (baseLocation + ".position").c_str());
        int colorLoc = GetShaderLocation(shader, (baseLocation + ".color").c_str());
        int strengthLoc = GetShaderLocation(shader, (baseLocation + ".strength").c_str());

        // Convert for use in shader
        int active = light.active ? 1 : 0;
        Vector3 position = light.position;
        Vector3 color = { light.color.r / 255.0f, light.color.g / 255.0f, light.color.b / 255.0f };
        float strength = light.strength;

        // Send to shader
        SetShaderValue(shader, activeLoc, &active, SHADER_UNIFORM_INT);
        SetShaderValue(shader, positionLoc, &position, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, colorLoc, &color, SHADER_UNIFORM_VEC3);
        SetShaderValue(shader, strengthLoc, &strength, SHADER_UNIFORM_FLOAT);
    }

}
