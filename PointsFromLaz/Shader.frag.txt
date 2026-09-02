#version 450

layout(location = 0) in vec3 fragNormal;
layout(location = 1) in vec3 fragPos;
layout(location = 2) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

layout(set = 0, binding = 0) uniform UniformBufferObject
{
    mat4 view;
    mat4 proj;
    vec4 cameraPosition;
    vec4 lightPosition;
    vec4 lightColor;
    vec4 lightParams;
} ubo;

layout(push_constant) uniform Push {
    mat4 model;
    vec4 objectColor;
    vec4 specularParams;
}   push;

layout(set = 1, binding = 0) uniform sampler2D texSampler;

void main()
{
    if (push.specularParams.z > 5.0)
    {
        outColor = vec4(push.objectColor.rgb, 1.0);
        return;
    }

    vec3 norm = normalize(fragNormal);
    vec3 lightDir = normalize(ubo.lightPosition.xyz - fragPos);
    vec3 viewDir = normalize(ubo.cameraPosition.xyz - fragPos);
    vec3 halfwayDir = normalize(lightDir + viewDir);

    vec3 ambient = ubo.lightParams.x * ubo.lightColor.xyz;
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = ubo.lightParams.y * diff * ubo.lightColor.xyz;
    float spec = pow(max(dot(norm, halfwayDir), 0.0), push.specularParams.y);
    vec3 specular = push.specularParams.x * spec * ubo.lightColor.xyz;

    vec4 texColor = texture(texSampler, fragTexCoord);
    vec3 lightingResult = (ambient + diffuse + specular);

    vec3 finalRBG = lightingResult * texColor.rgb * push.objectColor.rgb;
    outColor = vec4(finalRBG, texColor.a);
}
