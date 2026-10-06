#version 330
#define MAX_LIGHTS 50

struct Light
{
    bool lightOn;

    vec3 position;
    vec3 color;

    float strength;
};

in vec4 fragColor;
vec3 objectColor = vec3(fragColor.r, fragColor.g, fragColor.b);
in vec3 fragNormal;
vec3 normal = normalize(fragNormal);
in vec3 fragPosition;

uniform vec3 viewPos;

uniform Light lights[MAX_LIGHTS];
uniform int lightCount;

out vec4 finalColor;

vec3 GetLighting(Light light)
{
    // Setup
    vec3 lightDir = light.position - fragPosition;
    vec3 lightDirNormalized = normalize(lightDir);

    // Allow for the light to disapate the further away
    float dist = length(lightDir);
    float a = 8.0;
    float b = 0.7;
    float intensity = 1.0 / (a * dist + b * dist + 1.0);
    intensity *= light.strength;

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = (ambientStrength * light.color) * intensity;
    // Diffused lighting
    float diff = max(dot(normal, lightDirNormalized), 0.0f);
    vec3 diffuse = (diff * light.color) * intensity;
    // Specular lighting
    vec3 specular = vec3(0);

    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDirNormalized, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    specular = (specularStrength * spec * light.color) * intensity;
    
    // Add it all together
    return (ambient + diffuse + specular);
}

void main()
{
    vec3 totalLighting = vec3(0.0);

    for (int i = 0; i < lightCount; i++)
    {
        if (lights[i].lightOn)
        {
            totalLighting += GetLighting(lights[i]);
        }
    }

    vec3 result = objectColor * totalLighting;

    finalColor = vec4(result, 1.0);
}