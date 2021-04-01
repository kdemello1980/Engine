#ifndef SCENE_H
#define SCENE_H
#include "Mesh.h"

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

            /**
             * @brief 
             * 
             * @param mesh 
             */
            void addMesh(Mesh mesh);

            std::vector<Mesh> getMeshes();

        protected:


        private:
            Scene();
            static Scene* m_scene;
            std::vector<Mesh> m_meshes;
            // std::unordered_map<std::string, Mesh> m_meshes;
    };
}
#endif // SCENE_H
