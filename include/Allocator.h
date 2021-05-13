#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wnullability-completeness"
#pragma clang diagnostic ignored "-Wunused-variable"

#ifndef ALLOCATOR_H
#define ALLOCATOR_H

#include "types.h"
// #define VMA_RECORDING_ENABLED 1
#include "vk_mem_alloc.h"

namespace KMDM
{
    class Allocator
    {
        public:
            ~Allocator();
            static Allocator* getInstance();

            /**
             * @brief Get an AllocatedBuffer struct consisting of a VkBuffer and a VmaAllocation.
             * 
             * @param size VkDeviceSize
             * @param buffer_usage VkBufferUsageFlags
             * @param vma_usage VmaMemoryUsage
             * @return AllocatedBuffer 
             */
            AllocatedBuffer getVMABuffer(VkDeviceSize size, VkBufferUsageFlags buffer_usage,
                VmaMemoryUsage vma_usage);
             

            /**
             * @brief Get an AllocatedImage struct consisting of a VkImage and a VmaAllocation.
            * 
            * @param width uint32_t
            * @param height uint32_t
            * @param mip_level uint32_t
            * @param format VkFormat
            * @param tiling VkImageTiling
            * @param image_usage VkImageUsageFlags
            * @param memory_usage VmaMemoryUsage
            * @param memory_properties VkMemoryPropertyFlags
            * @return AllocatedImage 
            */
            AllocatedImage getVMAImage(uint32_t width, uint32_t height, uint32_t mip_level,
                VkFormat format, VkImageTiling tiling, VkImageUsageFlags image_usage, VmaMemoryUsage memory_usage,
                VkMemoryPropertyFlags memory_properties);

            /**
             * @brief Get the Allocator object.
             * 
             * @return VmaAllocator 
             */
            VmaAllocator getAllocator();

            /**
             * @brief Clean up an allocated buffer.
             * 
             * @param buffer AllocatedBuffer
             */
            void cleanupAllcatedBuffer(AllocatedBuffer buffer);

            /**
             * @brief 
             * 
             * @param src AllocatedBuffer
             * @param dst AllocatedBuffer
             * @param size size_t
             */
            void copyVMABuffer(AllocatedBuffer src, AllocatedBuffer dst, size_t size);

            /**
             * @brief Destory the Allocator.
             * 
             */
            void destoryAllocator();

            /**
             * @brief Pad the size of a uniform buffer so that it fits the minimum alignment of the GPU.
             * 
             * @param initial_size 
             * @return size_t 
             */
            size_t padUniformBuffer(size_t initial_size);

        protected:

        private:
            Allocator();
            static Allocator* m_allocator;

            // VMA allocator;
            VmaAllocator m_vmaAlocator;
    };
}
#endif // ALLOCATOR_H
