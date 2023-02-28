#version 330 core

in vec3 vertex_position_worldspace;
in vec3 vertex_position_cameraspace;
in vec3 vertex_normal_cameraspace;
in vec2 vertex_UV;
in vec4 vertex_position_lightspace1; // shading

uniform sampler2D shadowMapSampler1;
uniform sampler2D diffuseColorSampler;
uniform sampler2D specularColorSampler;


uniform int useTexture = 0;
uniform int useShadows;
uniform int useSpecularTexture = 0;
uniform mat4 V;


// Phong 
// light properties
struct Light {
    vec4 La;
    vec4 Ld;
    vec4 Ls;
    vec3 lightPosition_worldspace;
    float power;
};
uniform Light light1;

// materials
struct Material {
    vec4 Ka; 
    vec4 Kd;
    vec4 Ks;
    float Ns; 
};

uniform Material mtl;

out vec4 fragmentColor;

vec4 phong(float visibility, Light light);
float ShadowCalculation(vec4 fragPositionLightspace, Light light,sampler2D diffuseColorSampler);



void main() {   
    
    // Task 4.3
    float visibility1;
    
    if (useShadows==1){
        float shadow1  = ShadowCalculation(vertex_position_lightspace1,light1,shadowMapSampler1);
        visibility1 = 1.0f - shadow1;
    }
    else visibility1 = 1.0f;
    
    fragmentColor = phong(visibility1,light1);
}



float ShadowCalculation(vec4 vertexPositionLightspace, Light light,sampler2D shadowMapSampler){
    // Given the position of a fragment in lightspace coordinates
    // We sample the depth buffer to see whether or not the fragment is shadowed
    
    float shadow;

    // Task 4.3
    
    // Perspective devide to bring coordinates in range[-1, 1]
    // transform NDS coordinates ([-1,1]) to texture coordinates ([0,1])
    vec3 projCoords = vertexPositionLightspace.xyz / vertexPositionLightspace.w;

    // Since the depth map values are in range[0, 1]
    projCoords = projCoords * 0.5 + 0.5;

    // Sampling the closest point in this position from the depth map
    // REMINDER: Since we are in lightspace coordinates,
    //           the z parameter is the depth from the camera
    float closestDepth = texture(shadowMapSampler, projCoords.xy).r;

    // Then we get the depth of the current vertex
    float currentDepth = projCoords.z;

    // If the currentDepth is larger than the closestDepth, the fragment is shadowed
    shadow = currentDepth > closestDepth ? 1.0 : 0.0;
  
    // Task 4.4
    // Correcting the quantization problem
    float bias = 0.005;
    shadow = currentDepth - bias > closestDepth ? 1.0 : 0.0;
        

    // Task 4.7 Make the shadow edges more realistic
    
    shadow = 0.0;
    vec2 depthMap_dimensions = textureSize(shadowMapSampler, 0);
    vec2 texelSize = 1.0 / depthMap_dimensions;
    for(int x = -1; x <= 1; x++ ){
        for(int y = -1; y <= 1; y++ ){
            float pcfDepth = texture(shadowMapSampler, projCoords.xy + vec2(x, y) * texelSize).r;
            shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0; 
        }
    }
    shadow /= 9.0;
    


    // Task 4.5
    /*/
    if(projCoords.z > 1.0)
        shadow = 0.0;
    //*/


    return shadow;

}


vec4 phong(float visibility, Light light) {
   
    vec4 _Ks = mtl.Ks;
    vec4 _Kd = mtl.Kd;
    vec4 _Ka = mtl.Ka;
    float _Ns = mtl.Ns;

    // use texture for materials
    if (useTexture == 1) {
        if (useSpecularTexture == 1 ) _Ks = 0.1*vec4(texture(specularColorSampler, vertex_UV).rgb, 1.0);
        _Kd = 0.6*vec4(texture(diffuseColorSampler, vertex_UV).rgb, 1.0);
        _Ka = 0.5*vec4(texture(diffuseColorSampler, vertex_UV).rgb, 1.0);
        _Ns = 10;
    }
    
    // model ambient intensity (Ia)
    vec4 Ia = light.La * _Ka;

    // model diffuse intensity (Id)
    vec3 N = normalize(vertex_normal_cameraspace); 
    vec3 L = normalize((V * vec4(light.lightPosition_worldspace, 1)).xyz - vertex_position_cameraspace);
    float cosTheta = clamp(dot(L, N), 0, 1);
    vec4 Id = light.Ld * _Kd * cosTheta; 

    // model specular intensity (Is)
    vec3 R = reflect(-L, N);
    vec3 E = normalize(- vertex_position_cameraspace);
    float cosAlpha = clamp(dot(E, R), 0, 1);
    float specular_factor = pow(cosAlpha, _Ns);
    vec4 Is = light.Ls * _Ks * specular_factor;

    //model the light distance effect
    float distance = length(light.lightPosition_worldspace - vertex_position_worldspace);
    // float distance_sq = (distance * distance);
    float max_distance = 10.0f;
    float factor_of_distance = (distance/max_distance) * 0.5 + 0.5;
    

    // final fragment color
    // vec4 fragmentColor_ = vec4(
    //     Ia + 
    //     visibility * Id * light.power / distance_sq );

    // if (useSpecularTexture == 1) fragmentColor_ +=  visibility * Is * light.power / distance_sq;
    // return fragmentColor_;

    vec4 fragmentColor_ = vec4(
        Ia + 
        visibility * Id * light.power / factor_of_distance );

    if (useSpecularTexture == 1) fragmentColor_ +=  visibility * Is * light.power /factor_of_distance  ;
    return fragmentColor_;
    
}