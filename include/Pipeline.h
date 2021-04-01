#ifndef PIPELINE_H
#define PIPELINE_H

#include "Common.h"
#include "LogicalDevice.h"
#include "Renderpass.h"
#include "Pipeline.h"

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

        protected:
            void createGraphicsPipeline();

        private:
            VkPipeline m_graphicsPipeline;
            VkPipelineLayout m_graphicsPipelineLayout;
            LogicalDevice* m_logicalDevice;
            Renderpass* m_renderPass;
    };
}
#endif // PIPELINE_H
