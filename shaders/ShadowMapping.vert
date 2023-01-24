#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
uniform mat4 lightVP1;


out vec3 vertex_position_worldspace;
out vec3 vertex_position_cameraspace;
out vec3 vertex_normal_cameraspace;
out vec2 vertex_UV;
out vec4 vertex_position_lightspace1;


void main() {

    // Output position of the vertex
    gl_Position =  P * V * M * vec4(vertexPosition_modelspace, 1);
    
    // FS
    vertex_position_worldspace = (M * vec4(vertexPosition_modelspace, 1)).xyz;
    vertex_position_cameraspace = (V * M * vec4(vertexPosition_modelspace, 1)).xyz;
    vertex_normal_cameraspace = (V * M * vec4(vertexNormal_modelspace, 0)).xyz; 
    vertex_UV = vertexUV;

    // Task 4.2
    vertex_position_lightspace1 = lightVP1 * M * vec4(vertexPosition_modelspace, 1.0f);
}