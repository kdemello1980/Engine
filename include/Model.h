#ifndef MODEL_H
#define MODEL_H

#include "Common.h"
#include "Util.h"

#include <vulkan/vulkan.h>
#include <string>

namespace KMDM
{
    class Model
    {
        public:
            Model(std::string model_path, std::string texture_path);
            virtual ~Model();

            VkBuffer getVertexBuffer();
            VkBuffer getIndexBuffer();
            void destroyModel();

            VkImageView getTextureImageView();
            VkSampler getTextureSampler();

            uint32_t getIndexCount();

        protected:
            void loadModel(std::string path);
            void loadTexture(std::string path);
            void createVertexBuffer();
            void createIndexBuffer();
            void createTextureImage(std::string path);

            void generateMipmaps(VkImage image, int32_t tex_width, int32_t tex_height, 
                VkFormat format, uint32_t mip_levels);

            void transitionImageLayout(VkImage image, VkFormat format,
                VkImageLayout old_layout, VkImageLayout new_layout, uint32_t mip_levels);

            void createTextureImageView();
            void createTextureSampler();

        private:
            // Vertex buffer
            VkBuffer m_vertexBuffer;
            std::vector<Vertex> m_vertices;
            VkDeviceMemory m_vertexBufferMemory;

            // Indices
            std::vector<uint32_t> m_indices;
            VkBuffer m_indexBuffer;
            VkDeviceMemory m_indexBufferMemory;

            // Texture
            VkImage m_textureImage;
            VkDeviceMemory m_textureImageMemory;
            VkImageView m_textureImageView;
            VkSampler m_textureImageSampler;
            uint32_t m_mipLevels;
    };
}


#endif // MODEL_H
