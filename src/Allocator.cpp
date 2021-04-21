#include "Allocator.h"
#include "PhysicalDevice.h"
#include "LogicalDevice.h"
#include "Instance.h"
#include "Util.h"


#define VMA_IMPLEMENTATION
#include "vk_mem_alloc.h"

#include <stdexcept>

namespace KMDM
{

    Allocator* Allocator::m_allocator = nullptr;

    /*
        Allocator()
    */
    Allocator::Allocator()
    {
        VmaAllocatorCreateInfo allocatorInfo = {};
        allocatorInfo.physicalDevice = PhysicalDevice::getInstance()->getPhysicalDevice();
        allocatorInfo.device = LogicalDevice::getInstance()->getLogicalDevice();
        allocatorInfo.instance = Instance::getInstance()->getVulkanInstance();

        vmaCreateAllocator(&allocatorInfo, &m_vmaAlocator);
    }

    /*
        ~Allocator()
    */
    Allocator::~Allocator()
    {
        destoryAllocator();
    }

    /*
        getInstance()
    */
    Allocator* Allocator::getInstance()
    {
        if (!m_allocator)
        {
            m_allocator = new Allocator();
        }
        return m_allocator;
    }

    /*
        getAllocator()
    */
    VmaAllocator Allocator::getAllocator()
    {
        return m_vmaAlocator;
    }


    AllocatedBuffer Allocator::getVMABuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage,
        VmaMemoryUsage vma_usage)
    {
        VkBufferCreateInfo bufferInfo = {};
        bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
        bufferInfo.size = size;
        bufferInfo.usage = vma_usage;

        VmaAllocationCreateInfo vmaallocInfo = {};
        vmaallocInfo.usage = vma_usage;

        AllocatedBuffer ret = {};

        if (vmaCreateBuffer(m_vmaAlocator, &bufferInfo, &vmaallocInfo, &ret.buffer,
            &ret.allocation, nullptr) != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate VMA buffer.");
        }
        return ret;
    }


    void Allocator::copyVMABuffer(AllocatedBuffer src, AllocatedBuffer dst, size_t size)
    {
        copyBuffer(src.buffer, dst.buffer, size);
    }

    AllocatedImage Allocator::getVMAImage(uint32_t width, uint32_t height, uint32_t mip_level,
        VkFormat format, VkImageTiling tiling, VkImageUsageFlags image_usage, VmaMemoryUsage memory_usage,
        VkMemoryPropertyFlags memory_properties)
    {
        // Create image info.
        VkImageCreateInfo imageInfo = {};
        imageInfo.sType = VK_STRUCTURE_TYPE_IMAGE_CREATE_INFO;
        imageInfo.imageType = VK_IMAGE_TYPE_2D;
        imageInfo.extent.width = static_cast<uint32_t>(width);
        imageInfo.extent.height = static_cast<uint32_t>(height);
        imageInfo.extent.depth = 1;
        imageInfo.mipLevels = mip_level;
        imageInfo.arrayLayers = 1;
        imageInfo.format = format; // Use the same format for texels as pixels
        imageInfo.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
        imageInfo.tiling = tiling;
        imageInfo.usage = image_usage;
        imageInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
        imageInfo.samples = VK_SAMPLE_COUNT_1_BIT;

        VmaAllocationCreateInfo allocInfo = {};
        allocInfo.usage = memory_usage;
        allocInfo.requiredFlags = memory_properties;

        AllocatedImage retImage = {};
        if (vmaCreateImage(m_vmaAlocator, &imageInfo, &allocInfo, &retImage.image, &retImage.allocation, nullptr)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate image.");
        }
        return retImage;
    }


    void Allocator::cleanupAllcatedBuffer(AllocatedBuffer buffer)
    {
        vmaDestroyBuffer(m_vmaAlocator, buffer.buffer, buffer.allocation);
    }


    void Allocator::destoryAllocator()
    {
        vmaDestroyAllocator(m_vmaAlocator);
        m_allocator = nullptr;
    }

    size_t Allocator::padUniformBuffer(size_t initial_size)
    {
        size_t alignedSize = initial_size;
        VkPhysicalDeviceProperties deviceProperties;
        vkGetPhysicalDeviceProperties(PhysicalDevice::getInstance()->getPhysicalDevice(),
            &deviceProperties);
        
        size_t minUboAlignment = deviceProperties.limits.minUniformBufferOffsetAlignment;
        if (minUboAlignment > 0)
        {
            alignedSize = (alignedSize + minUboAlignment - 1) & ~(minUboAlignment - 1);
        }
        return alignedSize;
    }
}

