#version 330 core

// input vertex, UV coordinates and normal
layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;
layout (location = 3) in mat4 aInstanceMatrix; // translation matrix
layout (location = 7) in mat4 rotationMatrix;
layout (location = 11) in float scale;
layout (location = 12) in float life;


out vec2 UV;
out float vs_life;

//out vec3 normal;

// Values that stay constant for the whole mesh.
uniform mat4 PV;

void main() {
    // vertex position

    UV = vertexUV;

    gl_Position =  PV * aInstanceMatrix * rotationMatrix * vec4(vertexPosition_modelspace * scale, 1);

    gl_PointSize = 10;

    vs_life = life;
}
