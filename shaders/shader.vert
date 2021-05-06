#version 450
#extension GL_ARB_separate_shader_objects : enable

// // Use an binding though a uniform buffer object
// layout(binding = 0) uniform UniformBufferObject
// {
//     mat4 model;
//     mat4 view;
//     mat4 proj;
// } ubo;

// Camera data
// layout(set = 0, binding = 0) uniform CameraBuffer {
//     mat4 view;
//     mat4 proj;
//     mat4 viewproj;
// } cameraData;

// Per pass binding.
//layout(set = 1, binding = 0);

layout(set = 0, binding = 0) uniform Vertex {
    vec4 position;
    vec3 normal;
    vec3 color;
    vec2 texCoord;
} vertex;

layout(push_constant) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} cameraData;

layout(set = 1, binding = 0) uniform UBO {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(set = 2, binding = 0) uniform World {
    vec4 fogColor;
    vec4 fogDistance;
    vec4 ambientColor;
    vec4 sunlightDirection;
    vec4 sunlightColor;
} world;

// Input locations.
// layout(location = 0) in vec3 inPosition;

// Output locationsl
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{

    fragColor = vertex.color;
    fragTexCoord = vertex.texCoord;
}

