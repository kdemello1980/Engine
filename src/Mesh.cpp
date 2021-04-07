#include "../include/Mesh.h"
#include "../include/types.h"
#include "../include/Util.h"
#include "../include/LogicalDevice.h"
#include "Allocator.h"

#include <string>
#include <stdexcept>
#include <vector>
#include <unordered_map>
#include <glm/glm.hpp>

#define TINYOBJLOADER_IMPLEMENTATION
#include <tiny_obj_loader.h>

namespace KMDM
{
    Mesh::Mesh(std::string path, const std::string name)
    {
        // Set the name.
        m_name = name;

        // Read in model object.
        loadModel(path);

        // Create vertex buffer.
        createVertexBuffer();

        // Create index buffer.
        createIndexBuffer();

        // Assign material.


        m_transformMatrix = glm::mat4{1.0f};
    }

    Mesh::Mesh(const Mesh &m)
    {
        m_indexBuffer = m.m_indexBuffer;
        m_vertexBuffer = m.m_vertexBuffer;
        m_indices = m.m_indices;
        m_vertices = m.m_vertices;
        m_material = m.m_material;
        m_name = m.m_name;
        m_transformMatrix = m.m_transformMatrix;
    }

    Mesh::~Mesh()
    {

    }


    /**
     * @brief Create the vertex buffer for the Mesh.
     * 
     */
    void Mesh::createVertexBuffer()
    {
        VkDeviceSize bufferSize = m_vertices.size() * sizeof(Vertex);
        // m_vertexBuffer = loadGpuBuffer(m_vertices.data(), bufferSize, 0, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT);
        m_vertexBuffer = Allocator::getInstance()->getVMABuffer(bufferSize, VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | 
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);
        
        // Load the data into the buffer.
        void *data;
        vmaMapMemory(Allocator::getInstance()->getAllocator(), m_vertexBuffer.allocation, &data);
        memcpy(data, m_vertices.data(), bufferSize);
        vmaUnmapMemory(Allocator::getInstance()->getAllocator(), m_vertexBuffer.allocation);
    }


    /**
     * @brief Create the index buffer for the Mesh.
     * 
     */
    void Mesh::createIndexBuffer()
    {
        VkDeviceSize bufferSize = m_indices.size() * sizeof(uint32_t);
        // m_indexBuffer = loadGpuBuffer(m_indices.data(), bufferSize, 0, VK_BUFFER_USAGE_INDEX_BUFFER_BIT);
        m_indexBuffer = Allocator::getInstance()->getVMABuffer(bufferSize, VK_BUFFER_USAGE_INDEX_BUFFER_BIT |
           VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VMA_MEMORY_USAGE_CPU_ONLY);

        // Load the data into the buffer.
        void *data;
        vmaMapMemory(Allocator::getInstance()->getAllocator(), m_indexBuffer.allocation, &data);
        memcpy(data, m_vertices.data(), bufferSize);
        vmaUnmapMemory(Allocator::getInstance()->getAllocator(), m_indexBuffer.allocation);
    }

    /**
     * @brief Load a model from the filesystem path.
     * 
     * @param path 
     */
    void Mesh::loadModel(std::string path)
    {
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn, err;

        if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str()))
        {
            throw std::runtime_error("Failed to load model.");
        }

        // Create an unordered map to store unique vertices in.
        std::unordered_map<Vertex, uint32_t> uniqueVertices = {};

        // Iterate over teh shapes an build our vetors of vertices, normals and
        // texture positions.
        for (auto & shape : shapes)
        {
            for (auto & index : shape.mesh.indices)
            {
                Vertex vertex = {};

                // Position.
                vertex.position =
                {
                    attrib.vertices[3 * index.vertex_index + 0],
                    attrib.vertices[3 * index.vertex_index + 1],
                    attrib.vertices[3 * index.vertex_index + 2]
                };

                // Normal.
                vertex.normal =
                {
                    attrib.normals[3 * index.normal_index + 0],
                    attrib.normals[3 * index.normal_index + 1],
                    attrib.normals[3 * index.normal_index + 2]
                };

                // Color.
                vertex.color = { 1.0f, 1.0f, 1.0f };

                // Texture coordinates.
                vertex.texCoord  =
                {
                    attrib.texcoords[2 * index.texcoord_index + 0],
                    1.0f - attrib.texcoords[2 * index.texcoord_index + 1]
                };

                if (uniqueVertices.count(vertex) == 0)
                {
                    uniqueVertices[vertex] = static_cast<uint32_t>(m_vertices.size());
                    m_vertices.push_back(vertex);
                }
                m_indices.push_back(uniqueVertices[vertex]);
            }
        }
    } // loadModel

    /**
     * @brief 
     * 
     * @return std::string 
     */
    std::string Mesh::getName()
    {
        return m_name;
    }

    /**
    * @brief Set the Name object
    * 
    * @param name 
    */
    void Mesh::setName(const std::string name)
    {
        m_name = name;
    }

    /**
     * @brief 
     * 
     * @param material 
     */
    void Mesh::setMaterial(Material material)
    {
        m_material = material;
    }

    /**
     * @brief 
     * 
     * @param transform 
     */
    void Mesh::setTransform(glm::mat4 transform)
    {
        m_transformMatrix = transform;
    }

    std::vector<Vertex> Mesh::getVertices()
    {
        return m_vertices;
    }

    std::vector<uint32_t> Mesh::getIndices()
    {
        return m_indices;
    }

    AllocatedBuffer Mesh::getVertexBuffer()
    {
        return m_vertexBuffer;
    }

    AllocatedBuffer Mesh::getIndexBuffer()
    {
        return m_indexBuffer;
    }
} // KMDM
