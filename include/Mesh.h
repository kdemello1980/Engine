#ifndef MESH_H
#define MESH_H

#include "types.h"
#include "Util.h"
#include "Allocator.h"
#include "Pipeline.h"

#include <glm/glm.hpp>
#include <vector>
#include <string>
#include <optional>

namespace KMDM
{
    class Mesh;
    // Mesh structure
    struct SceneMeshes
    {
        std::optional<std::vector<Mesh>> meshes;
    };

    class Mesh
    {
        public:
            Mesh(std::string path, const std::string name);
            Mesh(const Mesh &m);
            virtual ~Mesh();
            void destroyMesh();

            void loadModel(std::string path);

            std::vector<Vertex> getVertices();
            std::vector<uint32_t> getIndices();
            AllocatedBuffer getVertexBuffer();
            AllocatedBuffer getIndexBuffer();

            /**
             * @brief Get the Name object
             * 
             * @return std::string 
             */
            std::string getName();

            /**
             * @brief Set the Name object
             * 
             * @param name 
             */
            void setName(const std::string name);

            void setTransform(glm::mat4 transform);

        protected:
            void createVertexBuffer();
            void createIndexBuffer();

        private:
            // Vertex buffer.
            std::vector<Vertex> m_vertices;
            AllocatedBuffer m_vertexBuffer;

            // Index buffer.
            std::vector<uint32_t> m_indices;
            AllocatedBuffer m_indexBuffer;

            // Mesh name.
            std::string m_name;

            // Transform matrix.
            glm::mat4 m_transformMatrix;

            // Texture.
            AllocatedImage m_textureImage;
            VkImageView m_textureImageView;
    };
}
#endif // MESH_H
