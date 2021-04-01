#ifndef RENDERPASS_H
#define RENDERPASS_H

#include <vulkan/vulkan.h>
#include "types.h"

namespace KMDM
{
    class Renderpass
    {
        public:
            Renderpass();
            virtual ~Renderpass();
            VkRenderPass getRenderPass();
            VkFormat findDepthFormat();

        protected:

        private:
            VkRenderPass m_renderPass;
    };
}
#endif // RENDERPASS_H
