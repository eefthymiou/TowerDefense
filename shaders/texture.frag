#version 330 core

// Interpolated values from the vertex shaders
in vec2 UV;

// output data
out vec4 color;

uniform sampler2D textureSampler;


void main()
{
    vec4 main_texture = vec4(texture(textureSampler, UV).rgb, 1);
    color = main_texture;
}