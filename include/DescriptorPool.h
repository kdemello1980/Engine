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

        private:
            VkDescriptorPool m_descriptorPool;
    };
}


#endif