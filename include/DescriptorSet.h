
#ifndef DESCRIPTORSET_H
#define DESCRIPTORSET_H

#include "LogicalDevice.h"
#include "Renderpass.h"
#include "Model.h"


#include <vulkan/vulkan.h>
#include <vector>
#include <optional>

namespace KMDM
{

    class DescriptorSet
    {
        public:
            DescriptorSet(Renderpass *renderpass);
            ~DescriptorSet();
            void destoryDescriptorSet();

            VkDescriptorSetLayout getSceneLayout();
            VkDescriptorSetLayout getModelLayout();
            VkDescriptorPool getSceneDescriptorPool();
            VkDescriptorPool getModelDescriptorPool();

            std::vector<VkDescriptorSet> getSceneDescriptorSets(uint32_t count, std::vector<VkBuffer> gpuSceneBuffers,
                std::vector<VkBuffer> uniformBuffers);

            std::vector<VkDescriptorSet> getModelDescriptorSets(std::vector<Model> models);

            void resetDescriptorPools(); // Do this after every frame.



        protected:
            void createDescriptorPool(uint32_t numDescriptors, Renderpass* renderpass, 
                VkDescriptorPool* pool, std::vector<VkDescriptorPoolSize> sizes);
            
            void createScenePool();
            void createModelPool();

        private:

            LogicalDevice* m_logicalDevice;
            Renderpass* m_renderPass;    


            // Env Descriptor pool (Scene and UBO).
            VkDescriptorSetLayout m_sceneLayout;
            VkDescriptorPool m_sceneDescriptorPool;

            // Model Descriptor pool.
            VkDescriptorSetLayout m_modelLayout;
            VkDescriptorPool m_modelDescriptorPool;        
    };
}
#endif