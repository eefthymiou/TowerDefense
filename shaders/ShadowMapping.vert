#version 330 core

layout(location = 0) in vec3 vertexPosition_modelspace;
layout(location = 1) in vec3 vertexNormal_modelspace;
layout(location = 2) in vec2 vertexUV;
layout(location = 3) in int bone_id;

uniform mat4 P;
uniform mat4 V;
uniform mat4 M;
mat4 new_M;
uniform mat4 lightVP1;
uniform int is_animation = 0;
uniform mat4 bone_matrices[64];

out vec3 vertex_position_worldspace;
out vec3 vertex_position_cameraspace;
out vec3 vertex_normal_cameraspace;
out vec2 vertex_UV;
out vec4 vertex_position_lightspace1;


void main() {
    if (is_animation == 1){
        new_M = M * bone_matrices[bone_id];
    }
    else {
        new_M = M;
    }

     // Output position of the vertex
    gl_Position =  P * V * new_M * vec4(vertexPosition_modelspace, 1);
    // FS
    vertex_position_worldspace = (new_M * vec4(vertexPosition_modelspace, 1)).xyz;
    vertex_position_cameraspace = (V * new_M * vec4(vertexPosition_modelspace, 1)).xyz;
    vertex_normal_cameraspace = (V * new_M * vec4(vertexNormal_modelspace, 0)).xyz; 
    vertex_UV = vertexUV;

    // Task 4.2
    vertex_position_lightspace1 = lightVP1 * new_M * vec4(vertexPosition_modelspace, 1.0f);
}