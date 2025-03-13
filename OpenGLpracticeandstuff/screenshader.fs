#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D screenTexture;

const float offset = 1.0 / 300.0;
void main()
{

    vec2 offsets[9] = vec2[](
        vec2(-offset, offset),
        vec2(0.0f, offset),
        vec2(offset, offset),
        vec2(-offset, 0.0f),
        vec2(0.0f, 0.0f),
        vec2(offset, 0.0f),
        vec2(-offset, -offset),
        vec2(0.0f, -offset),
        vec2(offset, -offset)
    );

    // Sharpen
    //float kernel[9] = float[](
    //    -1, -1, -1,
    //    -1,  9, -1,
    //    -1, -1, -1
    //);
    
    // Blur
    //float kernel[9] = float[](
    //    1.0 / 16, 2.0 / 16, 1.0 / 16,
    //    2.0 / 16, 4.0 / 16, 2.0 / 16,
    //    1.0 / 16, 2.0 / 16, 1.0 / 16
    //);

    // Edge Detection
    //float kernel[9] = float[](
    //    1,  1,  1,
    //    1, -8,  1,
    //    1,  1,  1
    //);

    // Alt
    float kernel[9] = float[](
        -1, -1,  1,
        -1,  5, -1,
         1, -1, -1
    );

    vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(screenTexture, TexCoords.st + offsets[i]));
    }

    vec3 color = vec3 (0.0);
    for(int i = 0; i < 9; i++)
        color += sampleTex[i] * kernel[i];

    // Original color
    //vec3 color = texture(screenTexture, TexCoords).rgb;
    
    // Inversion
    //color = vec3(1.0 - color);

    // Grayscale
    //float average = 0.2126 * color.r + 0.7125 * color.g + 0.0772 * color.b;
    //color = vec3(average);
    
    // Final
    vec3 post = vec3(color); 

    FragColor = vec4(post, 1.0);
} 

