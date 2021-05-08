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

// Vertex.
layout(set = 0, binding = 1) uniform Vertex {
    vec4 position;
    vec3 normal;
    vec3 color;
    vec2 texCoord;
} vertex;

// Camera.
layout (push_constant) uniform CameraData {
    mat4 view;
    mat4 projection;
    mat4 viewproj;
} camera;

// Texture/Sampler.
// layout(set = 0, binding = 2) uniform sampler2D texture;

// Output locationsl
layout(location = 0) out vec3 fragColor;
layout(location = 1) out vec2 fragTexCoord;

void main()
{

    fragColor = vertex.color;
    fragTexCoord = vertex.texCoord;
}

