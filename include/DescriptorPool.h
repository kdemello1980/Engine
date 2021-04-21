#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#include "Common.h"
#include "DescriptorPool.h"

#include <vulkan/vulkan.h>
#include <vector>

namespace KMDM
{
    class DescriptorPool
    {
        public:
            DescriptorPool();
            virtual ~DescriptorPool();

            VkDescriptorPool getPool();
                    
            std::vector<VkDescriptorSet> getPerFrameSet();
            std::vector<VkDescriptorSet> getPerPassSet();
            std::vector<VkDescriptorSet> getPerMaterialSet();
            std::vector<VkDescriptorSet> getPerObjectSet();

            VkDescriptorSetLayout getPerFrameLayout();
            VkDescriptorSetLayout getPerPassLayout();
            VkDescriptorSetLayout getPerMaterialLayout();
            VkDescriptorSetLayout getPerObjectLayout();

        protected:
            /**
             * @brief Create a Per Frame Set object
             * 
             */
            void createPerFrameSet();

            /**
             * @brief Create a Per Pass Set object
             * 
             */
            void createPerPassSet();
            

            /**
             * @brief Create a Per Material Set object
             * 
             */
            void createPerMaterialSet();

            /**
             * @brief Create a Per Object Set object
             * 
             */
            void createPerObjectSet();

        private:
            VkDescriptorPool m_descriptorPool;

            // Per frame Descriptor sets.
            VkDescriptorSetLayout m_perFrameLayout;
            std::vector<VkDescriptorSet> m_perFrameSet;

            // Per pass sets.
            VkDescriptorSetLayout m_perPassLayout;
            std::vector<VkDescriptorSet> m_perPassSet;

            // Material resources.
            VkDescriptorSetLayout m_materialLayout;
            std::vector<VkDescriptorSet> m_materialSet;

            // Per object resources.
            VkDescriptorSetLayout m_objectLayout;
            std::vector<VkDescriptorSet> m_objectSet;
    };
}


#endif