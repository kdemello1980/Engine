#include "../include/Model.h"
#include "../include/Common.h"
#include "../include/Util.h"
#include "../include/LogicalDevice.h"
#include "../include/CommandPool.h"
#include "../include/PhysicalDevice.h"
#include "../include/types.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

#include <glm/glm.hpp>

namespace KMDM
{
    Model::Model(std::string model_path, std::string texture_path)
    {
        loadModel(model_path);
        createVertexBuffer();
        createIndexBuffer();
        createTextureImage(texture_path);
        createTextureImageView();
        createTextureSampler();

        m_transBufferObj.rotate = glm::mat4(1.0);
        m_transBufferObj.scale = glm::float32(1.0);
        m_transBufferObj.translate = glm::mat4(1.0);

        creatteTransBuffer();
    }

    void Model::destroyModel()
    {
        vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), m_vertexBuffer, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), m_vertexBufferMemory, nullptr);

        vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), m_indexBuffer, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), m_indexBufferMemory, nullptr);

        vkDestroySampler(LogicalDevice::getInstance()->getLogicalDevice(), m_textureImageSampler, nullptr);
        vkDestroyImageView(LogicalDevice::getInstance()->getLogicalDevice(), m_textureImageView, nullptr);
        vkDestroyImage(LogicalDevice::getInstance()->getLogicalDevice(), m_textureImage, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), m_textureImageMemory, nullptr);
    }

    Model::~Model()
    {
        // destroyModel();
    }

    /******************************************************************
        Load the model.
    *******************************************************************/
    void Model::loadModel(std::string path)
    {
        // Load the model with tinyobj
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
        {
            throw std::runtime_error(warn + err);
        }

        // Get the unique verticies from the attribute array and store them in a vertex
        // struct.
        std::unordered_map<Vertex, uint32_t> unique_vertices = {};
        for (const auto & shape : shapes)
        {
            for (const auto & index : shape.mesh.indices)
            {
                Vertex vertex = {};

                vertex.position =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                vertex.normal =
                {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                vertex.texCoord =
                {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                vertex.color = {1.0f, 1.0f, 1.0f};

                if (unique_vertices.count(vertex) == 0)
                {
                    unique_vertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                    m_vertices.push_back(vertex);
                }
                //vertices.push_back(vertex);
                m_indices.push_back(unique_vertices[vertex]);
            }
        }
    } /// loadModel

    /**
     * @brief Load the verticies into a buffer in device local memory.
     * 
     */
    void Model::createVertexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(m_vertices[0]) * m_vertices.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
                     VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
                     stagingBuffer, stagingMemory);

        // Copy vertex data to the buffer.
        void *data;
        vkMapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_vertices.data(), (size_t)bufferSize);
        vkUnmapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory);

        // Create transfer destination buffer.
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
                     VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_vertexBuffer, m_vertexBufferMemory);

        // Copy the buffer contents
        copyBuffer(stagingBuffer, m_vertexBuffer, bufferSize);
        vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, nullptr);
    } /// createVertexBuffer

    /**
     * @brief Create the index buffer and load it into device memory.
     * 
     */
    void Model::createIndexBuffer()
    {
        VkDeviceSize bufferSize = sizeof(m_indices[0]) * m_indices.size();
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);
        
        void *data;
        vkMapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, 0, bufferSize, 0, &data);
        memcpy(data, m_indices.data(), (size_t)bufferSize);
        vkUnmapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory);
        
        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_indexBuffer, m_indexBufferMemory);

        copyBuffer(stagingBuffer, m_indexBuffer, bufferSize);

        vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, nullptr);
    }

    /**
     * @brief Create the translation buffer object.
     * 
     */
    void Model::creatteTransBuffer()
    {
        VkDeviceSize bufferSize = sizeof(m_transBufferObj);
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingMemory;

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, stagingBuffer, stagingMemory);
        
        void *data;
        vkMapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, 0, bufferSize, 0, &data);
        memcpy(data, &m_transBufferObj, (size_t)bufferSize);
        vkUnmapMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory);

        createBuffer(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT |
            VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, 
            m_translationBufffer, m_translationMemory);
        
        copyBuffer(stagingBuffer, m_translationBufffer, bufferSize);

        vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), stagingBuffer, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), stagingMemory, nullptr);
    }

    /******************************************************************
        Create the texture image.
    *******************************************************************/
    void Model::createTextureImage(std::string path)
    {
        int tex_width, tex_height, tex_channels;

        // Load the image.
        stbi_uc *pixels = stbi_load(path.c_str(), &tex_width, &tex_height,
                                    &tex_channels, STBI_rgb_alpha);
        if (!pixels)
        {
            throw std::runtime_error("Failed to load texture image.");
        }
        VkDeviceSize image_size = tex_width * tex_height * 4;  // 4 bytes per pixel
        m_mipLevels = static_cast<uint32_t>(std::floor(std::log2(std::max(tex_width, tex_height)))) + 1;

        // Staging buffer.
        VkBuffer staging_buffer;
        VkDeviceMemory staging_memory;
        createBuffer(image_size, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT |
                     VK_MEMORY_PROPERTY_HOST_COHERENT_BIT, staging_buffer, staging_memory);
        // Transfer data to staging buffer.
        void *data;
        vkMapMemory(LogicalDevice::getInstance()->getLogicalDevice(), staging_memory, 0, image_size, 0, &data);
        memcpy(data, pixels, static_cast<size_t>(image_size));
        vkUnmapMemory(LogicalDevice::getInstance()->getLogicalDevice(), staging_memory);

        // Free the pixel array.
        stbi_image_free(pixels);

        // Create the texture image.
        createImage(tex_width, tex_height, m_mipLevels, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_TILING_OPTIMAL,
                    VK_IMAGE_USAGE_TRANSFER_SRC_BIT | VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
                    VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
                    m_textureImage, m_textureImageMemory);

        // Copy the staging buffer to the texture image.
        transitionImageLayout(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED,
                              VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, m_mipLevels);

        // Copy the staging buffer to the image.
        copyBufferToImage(staging_buffer, m_textureImage, static_cast<uint32_t>(tex_width),
                          static_cast<uint32_t>(tex_height));

        // Generate the mipmaps.
        generateMipmaps(m_textureImage, tex_width, tex_height, VK_FORMAT_R8G8B8A8_SRGB, m_mipLevels);

        // Cleanup.
        vkDestroyBuffer(LogicalDevice::getInstance()->getLogicalDevice(), staging_buffer, nullptr);
        vkFreeMemory(LogicalDevice::getInstance()->getLogicalDevice(), staging_memory, nullptr);
    } /// createTextureImage

    /******************************************************************
        Create the texture image view.
    ******************************************************************/
    void Model::createTextureImageView()
    {
       m_textureImageView  = createImageView(m_textureImage, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_ASPECT_COLOR_BIT, m_mipLevels);
    } /// createTextureImageView

    /******************************************************************
        Generate mip maps.
    *******************************************************************/
    void Model::generateMipmaps(VkImage image, int32_t tex_width, int32_t tex_height, VkFormat format, uint32_t mip_levels)
    {
        // Check device capabilities.
        VkFormatProperties format_properties;
        vkGetPhysicalDeviceFormatProperties(PhysicalDevice::getInstance()->getPhysicalDevice(), format, &format_properties);
        if (!(format_properties.optimalTilingFeatures & VK_FORMAT_FEATURE_SAMPLED_IMAGE_FILTER_LINEAR_BIT))
        {
            throw std::runtime_error("Texture image format does not support linear blitting.");
        }

        // Get a command buffer
        VkCommandBuffer command_buffer = CommandPool::getInstance()->beginSingleTimeCommands();

        // Set up an image barrier.
        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.image = image;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;
        barrier.subresourceRange.levelCount = 1;

        int32_t mip_width = tex_width;
        int32_t mip_height = tex_height;

        for (uint32_t i = 1; i < mip_levels; i++)
        {
            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;

            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.subresourceRange.baseMipLevel = i - 1;

            vkCmdPipelineBarrier(command_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT,VK_PIPELINE_STAGE_TRANSFER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            // Blit info.
            VkImageBlit blit = {};
            blit.srcOffsets[0] = {0, 0, 0};
            blit.srcOffsets[1] = { mip_width, mip_height, 1 };
            blit.srcSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.srcSubresource.mipLevel = i - 1;
            blit.srcSubresource.baseArrayLayer = 0;
            blit.srcSubresource.layerCount = 1;

            blit.dstOffsets[0] = {0, 0, 0};
            blit.dstOffsets[1] = { mip_width > 1 ? mip_width / 2 : 1, mip_height > 1 ? mip_height / 2 : 1, 1};
            blit.dstSubresource.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
            blit.dstSubresource.mipLevel = i;
            blit.dstSubresource.baseArrayLayer = 0;
            blit.dstSubresource.layerCount = 1;

            vkCmdBlitImage(command_buffer,
                           image, VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL,
                           image, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL,
                           1, &blit,
                           VK_FILTER_LINEAR);

            barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_SRC_OPTIMAL;
            barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_READ_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            vkCmdPipelineBarrier(command_buffer,
                                 VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                                 0, nullptr,
                                 0, nullptr,
                                 1, &barrier);

            if  (mip_width > 1)
                mip_width /= 2;
            if (mip_height > 1)
                mip_height /= 2;
        }

        barrier.subresourceRange.baseMipLevel = mip_levels - 1;
        barrier.oldLayout = VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL;
        barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
        barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

        vkCmdPipelineBarrier(command_buffer,
                             VK_PIPELINE_STAGE_TRANSFER_BIT, VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT, 0,
                             0, nullptr,
                             0, nullptr,
                             1, &barrier);

        CommandPool::getInstance()->endSingleTimeCommands(command_buffer);
    } /// generateMipmaps


    /******************************************************************
        Transition image layout.
    *******************************************************************/
    void Model::transitionImageLayout(VkImage image, VkFormat format,
        VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_levels)
    {

        // Get a command buffer.
        VkCommandBuffer buffer = CommandPool::getInstance()->beginSingleTimeCommands();

        VkImageMemoryBarrier barrier = {};
        barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        barrier.oldLayout = old_layout;
        barrier.newLayout = new_layout;
        barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;

        barrier.image = image;
        barrier.subresourceRange.aspectMask = VK_IMAGE_ASPECT_COLOR_BIT;
        barrier.subresourceRange.baseMipLevel = 0;
        barrier.subresourceRange.levelCount = mip_levels;
        barrier.subresourceRange.baseArrayLayer = 0;
        barrier.subresourceRange.layerCount = 1;

        VkPipelineStageFlags source_stage;
        VkPipelineStageFlags dest_stage;
        if (old_layout == VK_IMAGE_LAYOUT_UNDEFINED && new_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL)
        {
            barrier.srcAccessMask = 0;
            barrier.dstAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;

            source_stage = VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT;
            dest_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;

        }
        else if (old_layout == VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL &&
                 new_layout == VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL)
        {
            barrier.srcAccessMask = VK_ACCESS_TRANSFER_WRITE_BIT;
            barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

            source_stage = VK_PIPELINE_STAGE_TRANSFER_BIT;
            dest_stage = VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT;
        }
        else
        {
            throw std::invalid_argument("Unsupported layout transition.");
        }

        vkCmdPipelineBarrier(
            buffer,
            source_stage, dest_stage,
            0,
            0, nullptr,
            0, nullptr,
            1, &barrier
        );

        // End the command buffer.
        CommandPool::getInstance()->endSingleTimeCommands(buffer);
    } /// transitionImageLayout

    /******************************************************************
        Create texture sampler.
    *******************************************************************/
    void Model::createTextureSampler()
    {
        // Sampler create info
        VkSamplerCreateInfo sampler_info = {};
        sampler_info.sType = VK_STRUCTURE_TYPE_SAMPLER_CREATE_INFO;
        sampler_info.magFilter = VK_FILTER_LINEAR;
        sampler_info.minFilter = VK_FILTER_LINEAR;
        sampler_info.addressModeU = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeV = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.addressModeW = VK_SAMPLER_ADDRESS_MODE_REPEAT;
        sampler_info.anisotropyEnable = VK_TRUE;

        // Get maximum anisotropy level from the physical device.
        VkPhysicalDeviceProperties properties = {};
        vkGetPhysicalDeviceProperties(PhysicalDevice::getInstance()->getPhysicalDevice(), &properties);

        sampler_info.maxAnisotropy = properties.limits.maxSamplerAnisotropy;
        sampler_info.borderColor = VK_BORDER_COLOR_INT_OPAQUE_BLACK;
        sampler_info.unnormalizedCoordinates = VK_FALSE;
        sampler_info.compareEnable = VK_FALSE;
        sampler_info.compareOp = VK_COMPARE_OP_ALWAYS;
        sampler_info.mipmapMode = VK_SAMPLER_MIPMAP_MODE_LINEAR;
        sampler_info.mipLodBias = 0.0f;
        sampler_info.minLod = 0.0f;
        sampler_info.maxLod = static_cast<float>(m_mipLevels);

        // Create the texture sampler.
        if (vkCreateSampler(LogicalDevice::getInstance()->getLogicalDevice(), &sampler_info, nullptr, &m_textureImageSampler)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create texture sampler.");
        }
    } /// createTextureSampler

    uint32_t Model::getIndexCount() { return static_cast<uint32_t>(m_indices.size()); }
    VkBuffer* Model::getVertexBuffer() { return &m_vertexBuffer; }
    VkBuffer* Model::getIndexBuffer() { return &m_indexBuffer; }
    VkImageView Model::getTextureImageView() { return m_textureImageView; }
    VkSampler Model::getTextureSampler() { return m_textureImageSampler; }
    VkBuffer Model::getTranslationMatrix() { return m_translationBufffer; }
}
