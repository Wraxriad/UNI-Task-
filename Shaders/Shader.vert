#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
    vec4 cameraPostion;
    vec4 lightPostion;
    vec4 lightColor;
    vec4 lightParams;
} ubo;

layout(push_constant) uniform PushConstants
{
    mat4 model;
    vec4 objectColor;
    vec4 SpecParams;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragNormal;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec2 fragTexCoord;

void main()
{
vec4 wordPos = push.model * vec4(inPosition, 1.0);
gl_Position = ubo.proj * ubo.view * wordPos;

fragPos = wordPos.xyz;
fragNormal = mat3(transpose(inverse(push.model)))* inNormal;
fragTexCoord = inTexCoord;
}

