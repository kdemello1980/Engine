#ifndef SCENE_H
#define SCENE_H
#include "Model.h"

#include <unordered_map>
#include <vector>
#include <string>
#include <vulkan/vulkan.h>


namespace KMDM
{
    class Scene
    {
        public:
            static Scene* getInstance();
            virtual ~Scene();

            void destoryScene();
            void addMesh(Model model);

            std::vector<Model> getMeshes();

        protected:


        private:
            Scene();
            static Scene* m_scene;
            std::vector<Model> m_meshes;
            // std::unordered_map<std::string, Mesh> m_meshes;
            GPUSceneData m_sceneData;
    };
}
#endif // SCENE_H
