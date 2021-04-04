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
layout(set = 0, binding = 0) uniform CameraBuffer {
    mat4 view;
    mat4 proj;
    mat4 viewproj;
} cameraData;

// Per pass binding.
//layout(set = 1, binding = 0);

// Material binding.
layout(set = 2, binding = 0) uniform Texture {
    vec3 inColor;
    vec2 inTexCoord;
} texture;

// Object binding.
layout(set = 3, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
    vec3 inPosition;
} ubo;

// Input locations.
// layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

// Output locationsl
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{
    gl_Position = ubo.proj * ubo.view * ubo.model * vec4(ubo.inPosition, 1.0);
    fragColor = texture.inColor;
    fragTexCoord = texture.inTexCoord;
}

