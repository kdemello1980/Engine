#ifndef DESCRIPTORPOOL_H
#define DESCRIPTORPOOL_H

#include "Common.h"

#include <vulkan/vulkan.h>

namespace KMDM
{
    class DescriptorPool
    {
        public:
            DescriptorPool();
            virtual ~DescriptorPool();

            VkDescriptorPool getPool();
        
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
            VkDescriptorSet m_perFrameSet;

            // Per pass sets.
            VkDescriptorSetLayout m_perPassLayout;
            VkDescriptorSet m_perPassSet;

            // Material resources.
            VkDescriptorSetLayout m_materialLayout;
            VkDescriptorSet m_materialSet;

            // Per object resources.
            VkDescriptorSetLayout m_objectLayout;
            VkDescriptorSet m_objectSet;
    };
}


#endif