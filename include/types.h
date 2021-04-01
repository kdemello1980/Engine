#ifndef TYPES_H_INCLUDED
#define TYPES_H_INCLUDED

#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#define GLM_FORCE_CXX17
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/glm.hpp>
#include <glm/ext/matrix_transform.hpp>
#include <glm/gtx/hash.hpp>

#include <vulkan/vulkan.h>

#include "Common.h"

#include <array>
#include <optional>
#include "vk_mem_alloc.h"



namespace KMDM
{
 /******************************************************************************/

    // The size of the window as reported by SDL.
    struct ExtentSize
    {
        int width;
        int height;
    };

/******************************************************************************/

    /**
       Structure and helper functions to represent a vertex.
    */
    struct Vertex
    {
        glm::vec3 position;
        glm::vec3 normal;
        glm::vec3 color;
        glm::vec2 texCoord;

        bool operator==(const Vertex& other) const
        {
            return position == other.position && normal == other.normal && color == other.color
                && texCoord == other.texCoord;
        }

        
    };

/******************************************************************************/

    /**
        Uniform buffer object.
    */
    struct UniformBufferObject
    {
        glm::mat4 model;
        glm::mat4 view;
        glm::mat4 proj;
    };


    /**
     * @brief Structure to hold the camera data.
     * 
     */
    struct CameraData
    {
        glm::mat4 view;
        glm::mat4 projection;
        glm::mat4 viewproj;
    };

/******************************************************************************/

   /**
        struct queueFamilyInfo: struct that contains a queue family index
        along with the associated VkQueueFamilyProperties struct.
    */
    struct QueueFamilyInfo
    {
        std::optional<VkQueueFamilyProperties> qProperties;
        std::optional<uint32_t> graphicsFamilyIndex;
        std::optional<uint32_t> presentationFamilyIndex;
        std::optional<uint32_t> computeFamilyIndex;

        bool isComplete()
        {
            return graphicsFamilyIndex.has_value() && qProperties.has_value() &&
                   presentationFamilyIndex.has_value();
        }

        bool hasCompute()
        {
            return computeFamilyIndex.has_value();
        }
    };
/******************************************************************************/

    /**
        struct SwapChainSupportDetails
    */
    struct SwapChainSupportDetails
    {
        VkSurfaceCapabilitiesKHR capabilities;
        std::vector<VkSurfaceFormatKHR> formats;
        std::vector<VkPresentModeKHR> presentModes;
    };
/******************************************************************************/

    /**
        Synchonization objects.
    */
    struct FrameSyncObjects
    {
        std::vector<VkSemaphore> imageAvailableSemaphores;
        std::vector<VkSemaphore> renderFinishedSemaphores;
        std::vector<VkFence> inflightFences;
        std::vector<VkFence> imagesInFlight;
        size_t currentFrame = 0;

        void incrementFrame()
        {
            currentFrame = (currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
        }
    };

/******************************************************************************/

    /**
        Shader modules.
    */
    struct ShaderModules
    {
        std::optional<VkShaderModule> vertexShader;
        std::optional<VkShaderModule> tessellationShader;
        std::optional<VkShaderModule> geometryShader;
        std::optional<VkShaderModule> fragmentShader;
    };

/******************************************************************************/

    // Allocated buffer.
    struct AllocatedBuffer
    {
        VkBuffer buffer;
        VmaAllocation allocation;
    };

/******************************************************************************/

    // Allocated image.
    struct AllocatedImage
    {
        VkImage image;
        VmaAllocation allocation;
    };

/******************************************************************************/

    // Frame data.  Maybe I can make this a class.
    struct FrameData
    {
        // Global Data.
        AllocatedBuffer cameraData;
        VkDescriptorSet globalDescriptor;
    };

/******************************************************************************/

    // Material.
    struct Material
    {
        VkPipeline* pipeline;
        VkPipelineLayout* pipelineLayout;
    };

/******************************************************************************/
}

// Define a hashing function for the Vertex struct.
namespace std
{
    template<> struct hash<KMDM::Vertex>
    {
        size_t operator()(KMDM::Vertex const& vertex) const
        {
            return ((hash<glm::vec3>()(vertex.position) ^
                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
                   ((hash<glm::vec2>()(vertex.texCoord) << 1) ^
                   (hash<glm::vec3>()(vertex.normal) << 1));
        }
    };
}
#endif // TYPES_H_INCLUDED
//namespace std
//{
//    template<> struct hash<KMDM::Vertex>
//    {
//        size_t operator()(KMDM::Vertex const& vertex) const
//        {
//            return ((hash<glm::vec3>()(vertex.pos) ^
//                     (hash<glm::vec3>()(vertex.color) << 1)) >> 1) ^
//                   (hash<glm::vec2>()(vertex.texCoord) << 1);
//        }
//    };
//}
