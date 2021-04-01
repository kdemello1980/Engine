#ifndef UTIL_H
#define UTIL_H

#include "LogicalDevice.h"
#include "CommandPool.h"
#include "types.h"


#include <vulkan/vulkan.h>

namespace KMDM
{
    /**
     * @brief Create a VkBuffer object
     * 
     * @param size 
     * @param usage 
     * @param properties 
     * @param buffer 
     * @param buffer_memory 
     */
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory);


    /**
     * @brief Copy the content of the source VkBuffer to the destination VkBuffer.
     * 
     * @param src 
     * @param dst 
     * @param size 
     */
    void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size);

    /**
     * @brief Create a VkImage object
     * 
     * @param width 
     * @param height 
     * @param mip_level 
     * @param format 
     * @param tiling 
     * @param usage 
     * @param properties 
     * @param image 
     * @param memory 
     */
    void createImage(uint32_t width, uint32_t height, uint32_t mip_level,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
        VkImage &image, VkDeviceMemory &memory);
    
    /**
     * @brief 
     * 
     * @param data 
     * @param size 
     * @param memory_properties 
     * @param buffer_usage 
     * @param dest_buffer 
     * @param dest_memory 
     */
    void loadGpuBuffer(void *data, VkDeviceSize size, VkMemoryPropertyFlags memory_properties, VkBufferUsageFlags buffer_usage,
        VkBuffer dest_buffer, VkDeviceMemory dest_memory);

    /**
     * @brief 
     * 
     * @param buffer 
     */
    // void cleanupAllocatedBuffer(AllocatedBuffer buffer);

    /**
     * @brief 
     * 
     * @param type_filter 
     * @param properties 
     * @return uint32_t 
     */
    uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties);

    /**
     * @brief 
     * 
     * @param buffer 
     * @param image 
     * @param width 
     * @param height 
     */
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height);

    /**
     * @brief Create a Image View object
     * 
     * @param image 
     * @param format 
     * @param aspect_flags 
     * @param mip_levels 
     * @return VkImageView 
     */
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels);
}
#endif // UTIL_H
