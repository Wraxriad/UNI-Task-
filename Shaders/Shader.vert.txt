#version 450

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;
    vec4 cameraPosition;
    vec4 lightPosition;
    vec4 lightColor;
    vec4 lightParams;
} ubo;

layout(push_constant) uniform PushConstants {
    mat4 model;
    vec4 objectColor;
    vec4 SpecParams;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec3 fragPos;
layout(location = 2) out vec2 fragTexCoord;

void main() {
    vec4 worldPos = push.model * vec4(inPosition, 1.0);
    gl_Position = ubo.proj * ubo.view * worldPos;

    // --- POINT CLOUD SPECIFIC ---
    gl_PointSize = 4.0; // Makes the points visible!
    // ----------------------------

    fragPos = worldPos.xyz;
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}