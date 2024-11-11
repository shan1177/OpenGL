#version 330 core

struct Material
{
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light
{
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

uniform Material material;
uniform Light light;

out vec4 FragColor;

//in vec3 ourColor

//uniform float mixValue;

uniform vec3 viewPos;

// texture sampler
//uniform sampler2D texture1;
//uniform sampler2D texture2;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoords;

void main()
{
    //FragColor = texture(texture1, TexCoord);
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), mixValue);

    // Ambient
    vec3 ambient = light.ambient * texture(material.diffuse, TexCoords).rgb;      // Ambient and Diffuse texture are same

    // Diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * diff * texture(material.diffuse, TexCoords).rgb;

    // Specular
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * spec * texture(material.specular, TexCoords).rgb;

    vec3 result = ambient + diffuse + specular;
    FragColor = vec4(result, 1.0);

}