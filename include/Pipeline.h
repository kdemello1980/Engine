#ifndef PIPELINE_H
#define PIPELINE_H

#include "Common.h"
#include "LogicalDevice.h"
#include "Renderpass.h"
#include "Pipeline.h"
#include "DescriptorPool.h"

#include <vulkan/vulkan.h>

namespace KMDM
{
    class Pipeline
    {
        public:
            Pipeline(Renderpass* render_pass);
            virtual ~Pipeline();
            void destoryPipeline();

            VkPipeline* getPipeline();
            VkPipelineLayout* getPipelineLayout();
            DescriptorPool getDescriptorPool();

        protected:
            void createGraphicsPipeline();

        private:
        
            VkPipeline m_graphicsPipeline;
            VkPipelineLayout m_graphicsPipelineLayout;
            LogicalDevice* m_logicalDevice;
            Renderpass* m_renderPass;

            // Descriptor pool.
            DescriptorPool *m_descriptorPool;
    };
}
#endif // PIPELINE_H
