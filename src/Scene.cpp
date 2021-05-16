#include "../include/Scene.h"
#include "../include/Model.h"


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
        Model m = Model(std::string("models/Indonesian_statue.stl"), std::string("head"));
        m_meshes.push_back(m);
    }

    Scene::~Scene()
    {
        destoryScene();
    }

    /**
     * @brief Clean up all of the Mesh objects in the scene.
     * 
     */
    void Scene::destoryScene()
    {
        for (auto & mesh : m_meshes)
        {
            mesh.destroyModel();
        }    
        m_scene = nullptr;    
    }


    std::vector<Model> Scene::getMeshes()
    {
        return m_meshes;
    }

    /**
     * @brief 
    * 
    * @param mesh 
    */
    void Scene::addMesh(Model mesh)
    {
        m_meshes.push_back(mesh);
    }
}
