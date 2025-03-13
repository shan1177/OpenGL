#version 330 core
out vec4 FragColor;

in vec2 TexCoords;
in vec3 Normal;   
in vec3 FragPos;  

uniform sampler2D texture_diffuse1;
uniform vec3 lightPos;   
uniform vec3 viewPos;    
uniform vec3 lightColor; 
uniform vec3 objectColor;

const float shadowThreshold = 0.5;
const float highlightThreshold = 0.8;

void main()
{
    float ambientStrength = 0.6;
    vec3 ambient = ambientStrength * lightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    float diffuseLevel = 0.0;
    if (diff > highlightThreshold) {
        diffuseLevel = 1.0;
    } else if (diff > shadowThreshold) {
        diffuseLevel = 0.5;
    } else {
        diffuseLevel = 0.2;
    }

    vec3 diffuse = diffuseLevel * lightColor;
    vec3 lighting = (ambient + diffuse);

    vec3 textureColor = texture(texture_diffuse1, TexCoords).rgb;
    vec3 finalColor = lighting * textureColor * objectColor;

    FragColor = vec4(finalColor, 1.0);
}