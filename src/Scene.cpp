#include "../include/Scene.h"
#include "../include/Mesh.h"


#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdexcept>
#include <vulkan/vulkan.h>

namespace KMDM
{
    Scene* Scene::m_scene = nullptr;

    Scene* Scene::getInstance()
    {
        if (!m_scene)
        {
            m_scene = new Scene();
        }
        return m_scene;
    }

    Scene::Scene()
    {
        Mesh m = Mesh(std::string("models/Indonesian_statue.stl"), std::string("head"));
        m_meshes.push_back(m);
    }

    Scene::~Scene()
    {
        //dtor
    }


    std::vector<Mesh> Scene::getMeshes()
    {
        return m_meshes;
    }

    /**
     * @brief 
    * 
    * @param mesh 
    */
    void Scene::addMesh(Mesh mesh)
    {
        m_meshes.push_back(mesh);
    }
}
