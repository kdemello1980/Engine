#include "../include/Util.h"
#include "../include/Renderer.h"
#include "../include/CommandPool.h"
#include "../include/LogicalDevice.h"
#include "../include/PhysicalDevice.h"

#include <stdexcept>
#include <vulkan/vulkan.h>

namespace KMDM
{
    /******************************************************************
        Create the host-visible vertex staging buffer.
    *******************************************************************/
    void createBuffer(VkDeviceSize size, VkBufferUsageFlags usage,
        VkMemoryPropertyFlags properties, VkBuffer& buffer, VkDeviceMemory& buffer_memory)
    {
        // Buffer create info struct.
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = usage;
        bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
    //        buffer_info.flags = 0;

        // Create the buffer.
        if (vkCreateBuffer(LogicalDevice::getInstance()->getLogicalDevice(), &bufferInfo, nullptr, &buffer) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create buffer.");
        }

        // Memory requirements.
        VkMemoryRequirements memRequirements;
        vkGetBufferMemoryRequirements(LogicalDevice::getInstance()->getLogicalDevice(), buffer, &memRequirements);

        // Allocate the memory.
        VkMemoryAllocateInfo allocate_info = {};
        allocate_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        allocate_info.allocationSize = memRequirements.size;
        allocate_info.memoryTypeIndex = findMemoryType(memRequirements.memoryTypeBits, properties);

        if (vkAllocateMemory(LogicalDevice::getInstance()->getLogicalDevice(), &allocate_info, nullptr, &buffer_memory)
                != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate buffer memory.");
        }

        // Associate the buffer with the memory.
        vkBindBufferMemory(LogicalDevice::getInstance()->getLogicalDevice(), buffer, buffer_memory, 0);
    } /// createBuffer


    /******************************************************************
        Copy buffer contents.
    *******************************************************************/
    void copyBuffer(VkBuffer src, VkBuffer dst, VkDeviceSize size)
    {
        // Get a single use command buffer.
        VkCommandBuffer buffer = CommandPool::getInstance()->beginSingleTimeCommands();

        // Copy buffer from src to dst.
        VkBufferCopy copyRegion = {};
        copyRegion.dstOffset = 0;
        copyRegion.srcOffset = 0;
        copyRegion.size = size;
        vkCmdCopyBuffer(buffer, src, dst, 1, &copyRegion);

        // Clean up the single use command buffer.
        CommandPool::getInstance()->endSingleTimeCommands(buffer);
    } /// copyBuffer

    /******************************************************************
        Create an image.
    *******************************************************************/
     void createImage(uint32_t width, uint32_t height, uint32_t mip_level,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags usage, VkMemoryPropertyFlags properties,
        VkImage &image, VkDeviceMemory &memory)
    {

        // Create image info.
        VkImageCreateInfo image_info = {};
        image_info.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        image_info.imageType = VK_IMAGE_TYPE_2D;
        image_info.extent.width = static_cast<uint32_t>(width);
        image_info.extent.height = static_cast<uint32_t>(height);
        image_info.extent.depth = 1;
        image_info.mipLevels = mip_level;
        image_info.arrayLayers = 1;
        image_info.format = format; // Use the same format for texels as pixels
        image_info.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        image_info.tiling = tiling;
        image_info.usage = usage;
        image_info.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        image_info.samples = VK_SAMPLE_COUNT_1_BIT;

        // Create the image.
        if (vkCreateImage(LogicalDevice::getInstance()->getLogicalDevice(), &image_info, nullptr, &image) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture image.");
        }

        // Allocate image memory.
        VkMemoryRequirements mem_requirements;
        vkGetImageMemoryRequirements(LogicalDevice::getInstance()->getLogicalDevice(), image, &mem_requirements);

        VkMemoryAllocateInfo alloc_info = {};
        alloc_info.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
        alloc_info.allocationSize = mem_requirements.size;
        alloc_info.memoryTypeIndex = findMemoryType(mem_requirements.memoryTypeBits, properties);

        if (vkAllocateMemory(LogicalDevice::getInstance()->getLogicalDevice(), &alloc_info, nullptr, &memory) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image memory.");
        }
        vkBindImageMemory(LogicalDevice::getInstance()->getLogicalDevice(), image, memory, 0);
    }

    /******************************************************************
        Find an appropriate vertex buffer memory type.
    *******************************************************************/
    uint32_t findMemoryType(uint32_t type_filter, VkMemoryPropertyFlags properties)
    {
        // Query available types of memory.
        VkPhysicalDeviceMemoryProperties mem_properties;
        vkGetPhysicalDeviceMemoryProperties(PhysicalDevice::getInstance()->getPhysicalDevice(), &mem_properties);

        // Find a memory type that is suitable for the buffer, and if we are able
        // to write vertex data to that memory.
        for (uint32_t i = 0; i < mem_properties.memoryTypeCount; i++)
        {
            if ((type_filter & (1 << i)) && (mem_properties.memoryTypes[i].propertyFlags & properties))
            {
                return i;
            }
        }
        throw std::runtime_error("Failed to find a suitable memory type.");
    } /// findMemoryType

    /******************************************************************
        Copy buffer to image.
    *******************************************************************/
    void copyBufferToImage(VkBuffer buffer, VkImage image, uint32_t width, uint32_t height)
    {
        // Get a command buffer.
        VkCommandBuffer cmd_buffer = CommandPool::getInstance()->beginSingleTimeCommands();

        // Specify which part of the buffer is copied to which part of the image.
        VkBufferImageCopy region = {};
        region.bufferOffset = 0;
        region.bufferRowLength = 0;
        region.bufferImageHeight = 0;
        region.imageSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        region.imageSubresource.mipLevel = 0;
        region.imageSubresource.baseArrayLayer = 0;
        region.imageSubresource.layerCount = 1;
        region.imageOffset = {0, 0, 0};
        region.imageExtent = {width, height, 1};

        // Enqueue the copy command.
        vkCmdCopyBufferToImage(
            cmd_buffer,
            buffer,
            image,
            VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
            1,
            &region
        );

        // End command buffer.
        CommandPool::getInstance()->endSingleTimeCommands(cmd_buffer);
    } /// copyBufferToImage

    /******************************************************************
        Create the image view.
    ******************************************************************/
    VkImageView createImageView(VkImage image, VkFormat format, VkImageAspectFlags aspect_flags, uint32_t mip_levels)
    {
        // Image view create info.
        VkImageViewCreateInfo view_info = {};
        view_info.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
        view_info.image = image;
        view_info.viewType = VK_IMAGE_VIEW_TYPE_2D;
        view_info.format = format;
        view_info.subresourceRange.aspectMask = aspect_flags;
        view_info.subresourceRange.baseMipLevel = 0;
        view_info.subresourceRange.levelCount = mip_levels;
        view_info.subresourceRange.baseArrayLayer = 0;
        view_info.subresourceRange.layerCount = 1;

        // Create image view.
        VkImageView view;
        if (vkCreateImageView(LogicalDevice::getInstance()->getLogicalDevice(), &view_info,nullptr, &view) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture image view.");
        }
        return view;
    } /// createImageVkew

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
        VkBuffer dest_buffer, VkDeviceMemory dest_memory)
    {
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        // Create the staging buffer
        createBuffer(
            size, 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, 
            VK_MEMORY_PROPERTY_HOST_COHERENT_BIT | VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT,
            stagingBuffer, 
            stagingMemory
        );
        
        // Load the data.
        void* mem;
        vkMapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, 0, size, 0, &mem);
        memcpy(mem, data, size);
        vkUnmapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory);

        // Create the GPU buffer.
        AllocatedBuffer GPUBuffer = {};
        createBuffer(
            size,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | buffer_usage,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT | memory_properties,
            dest_buffer,
            dest_memory
        );

        // Copy the contents of the staging buffer.
        copyBuffer(stagingBuffer, GPUBuffer.buffer, size);

        // Nuke the staging buffer.
        vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, nullptr);
    }

    /**
     * @brief 
     * 
     * @param buffer 
     */
    // void cleanupAllocatedBuffer(AllocatedBuffer buffer)
    // {
    //     vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), buffer.buffer, nullptr);
    //     vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), buffer.bufferMemory, nullptr);
    // }
}
