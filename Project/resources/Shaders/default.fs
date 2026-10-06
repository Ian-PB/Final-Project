#version 330

in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 lightPos;

out vec4 finalColor;

void main()
{
    // Setup
    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 normal = normalize(fragNormal);
    vec3 objectColor = vec3(fragColor.r, fragColor.g, fragColor.b);
    vec3 lightColor = vec3(1.0);

    // ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    // Diffused lighting
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
    

    vec3 result = (ambient + diffuse) * objectColor;
    finalColor = vec4(result, 1.0);
}