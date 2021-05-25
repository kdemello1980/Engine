#version 450
#extension GL_ARB_separate_shader_objects : enable

// Scene data
layout(set = 0, binding = 0) uniform GPUSceneData {
    vec4 fogColor;
    vec4 fogDistance;
    vec4 ambientColor;
    vec4 sunlightDirection;
    vec4 sunlightColor;
} sceneData;

// Camera.
layout (set = 0, binding = 1) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// Vertex.
// layout(set = 0, binding = 1) uniform Vertex {
//     vec3 position;
//     vec3 normal;
//     vec3 color;
//     vec2 texCoord;
// } vertex;
layout(location = 0) in vec3 inPostion;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

// Texture/Sampler.
// layout(set = 0, binding = 2) uniform sampler2D texture;

// Output locationsl
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(inPostion, 1.0);
    fragColor = inColor;
    fragTexCoord = inTexCoord;
}

