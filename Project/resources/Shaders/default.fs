#version 330

in vec4 fragColor;
in vec3 fragNormal;
in vec3 fragPosition;

uniform vec3 lightPos;
uniform vec3 viewPos;

out vec4 finalColor;

void main()
{
    // Setup
    vec3 objectColor = vec3(fragColor.r, fragColor.g, fragColor.b);
    vec3 lightDir = normalize(lightPos - fragPosition);
    vec3 normal = normalize(fragNormal);
    vec3 lightColor = vec3(1.0, 0.56, 0.27);

    // Ambient lighting
    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;
    // Diffused lighting
    float diff = max(dot(normal, lightDir), 0.0f);
    vec3 diffuse = diff * lightColor;
    // Specular lighting
    float specularStrength = 0.5;
    vec3 viewDir = normalize(viewPos - fragPosition);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;
    
    // Add it all together
    vec3 result = (ambient + diffuse + specular) * objectColor;
    finalColor = vec4(result, 1.0);
}