#ifndef LOGICALDEVICE_H
#define LOGICALDEVICE_H

#include <vulkan/vulkan.h>
#include <optional>
#include "types.h"

namespace KMDM
{
    class LogicalDevice
    {
        public:
            static LogicalDevice* getInstance();
            VkDevice getLogicalDevice();
            VkQueue getGraphicsQueue();
            VkQueue getPresentationQueue();
            QueueFamilyInfo getQueueFamilyInfo();
            virtual ~LogicalDevice();
            void destroyLogicalDevice();

            VkShaderModule createShaderModule(const std::vector<char>& code);

        protected:
            void getQueueHandle();
            void findSuitableQueueFamily(VkQueueFlags QUEUE_FLAGS);


        private:
            LogicalDevice();

            static LogicalDevice* m_logicalDevice;
            static VkDevice m_VKDevice;
            VkQueue m_graphicsQueue;
            VkQueue m_presentationQueue;
            QueueFamilyInfo m_queueFamilyInfo;
    };
}
#endif // LOGICALDEVICE_H
