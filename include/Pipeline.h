#ifndef PIPELINE_H
#define PIPELINE_H

#include "Common.h"
#include "LogicalDevice.h"
#include "Renderpass.h"
#include "Pipeline.h"
#include "DescriptorSet.h"

#include <vulkan/vulkan.h>

namespace KMDM
{
    class Pipeline
    {
        public:
            Pipeline(Renderpass* render_pass, DescriptorSet* descriptor_set);
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

            // Descriptor set layout.
            DescriptorSet* m_descriptorSet;
    };
}
#endif // PIPELINE_H
