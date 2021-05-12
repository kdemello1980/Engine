#ifndef COMMON_H_INCLUDED
#define COMMON_H_INCLUDED

// #pragma once

#include <vulkan/vulkan.h>

#include <vector>
#include <optional>
#include <string>


#ifdef NDEBUG
const bool enableValidationLayers = false;
#else
const bool enableValidationLayers = true;
#endif // NDEBUG

/******************************************************************
    Validation layers.
*******************************************************************/
const std::vector<const char *> VALIDATION_LAYERS =
{
    "VK_LAYER_KHRONOS_validation"
};

/******************************************************************
    Required device extensions.
*******************************************************************/
const std::vector<const char *> REQUIRED_DEVICE_EXTENSIONS =
{
    VK_KHR_SWAPCHAIN_EXTENSION_NAME,
};

/******************************************************************
    Globals and #defines
*******************************************************************/
const size_t MAX_FRAMES_IN_FLIGHT = 2;
// const size_t MAX_DESCRIPTOR_SETS = 12;
// const size_t DESCRIPTOR_CATEGORIES = 4;
// const size_t DESCRIPTOR_POOL_ENGINE_RESOURCES = 0;
// const size_t DESCRIPTOR_POOL_PER_PASS_RESOURCES = 1;
// const size_t DESCRIPTOR_POOL_MATERIAL_RESOURCES = 2;
// const size_t DESCRIPTOR_POOL_OBJECT_RESORUCES = 3;

const uint32_t MAX_DESCRIPTORS = 3072;

#define SHADER_PATH "shaders/"
#define WIDTH 1600
#define HEIGHT 1200
#define ENGINE "KMDMEngine"

const VkQueueFlags QUEUE_FLAGS = VK_QUEUE_COMPUTE_BIT | VK_QUEUE_GRAPHICS_BIT;

namespace KMDM
{

    char *string2char(std::string in);
    std::vector<char> readFile(const std::string& filename);

    bool loadJsonConfig(std::string path);
}

#endif // COMMON_H_INCLUDED
