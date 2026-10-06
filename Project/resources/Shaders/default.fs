#version 330

in vec4 fragColor;
vec3 objectColor = vec3(fragColor.r, fragColor.g, fragColor.b);
in vec3 fragNormal;
vec3 normal = normalize(fragNormal);
in vec3 fragPosition;

uniform vec3 lightPos;
vec3 lightDir = lightPos - fragPosition;
uniform vec3 viewPos;

out vec4 finalColor;

vec3 GetLighting(vec3 lightColor, bool withSpecular)
{
    // Setup
    vec3 lightDirNormalized = normalize(lightDir);

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    // Diffused lighting
    float diff = max(dot(normal, lightDirNormalized), 0.0f);
    vec3 diffuse = diff * lightColor;
    // Specular lighting
    vec3 specular = vec3(0);

    if (withSpecular)
    {
        float specularStrength = 0.5;
        vec3 viewDir = normalize(viewPos - fragPosition);
        vec3 reflectDir = reflect(-lightDirNormalized, normal);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
        specular = specularStrength * spec * lightColor;
    }
    
    // Add it all together
    return (ambient + diffuse + specular);
}

void main()
{
    // Colors
    vec3 playerLightColor = vec3(1.0, 0.56, 0.27);
    int lightCutoffMin = 5;
    int lightCutoffMax = 20;
    vec3 distantLightColor = vec3(0.09);

    vec3 lighting = GetLighting(playerLightColor, true);
    vec3 distantLighting = GetLighting(distantLightColor, false);
    float lightDist = length(lightDir);

    // Blend between the 2 colors
    float blend = smoothstep(lightCutoffMin, lightCutoffMax, lightDist);
    vec3 blendedLighting = mix(lighting, distantLighting, blend);

    vec3 result = objectColor * blendedLighting;

    finalColor = vec4(result, 1.0);
}