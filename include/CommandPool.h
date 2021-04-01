#ifndef COMMANDPOOL_H
#define COMMANDPOOL_H

#include <vulkan/vulkan.h>

namespace KMDM
{
    class CommandPool
    {
        public:
            /**
             * @brief Destroy the Command Pool object
            * 
            */
            virtual ~CommandPool();

            /**
             * @brief Get the Instance object
             * 
             * @return CommandPool* 
             */
            static CommandPool* getInstance();

            /**
             * @brief destroy the VkCommandPool object.
             * 
             */
            void destroyCommandPool();

            /**
             * @brief Get the Command Pool object
             * 
             * @return VkCommandPool 
             */
            VkCommandPool getCommandPool();

            /**
             * @brief Get a single time command buffer.
             * 
             * @return VkCommandBuffer 
             */
            VkCommandBuffer beginSingleTimeCommands();

            /**
             * @brief Stop buffer recording and submit to the graphics queue.
             * 
             * @param buffer 
             */
            void endSingleTimeCommands(VkCommandBuffer buffer);

        protected:

        private:
            /**
             * @brief Construct a new Command Pool object
             * 
             */
            CommandPool();

            /**
             * @brief Singleton CommandPool pointer.
             * 
             */
            static CommandPool* m_commandPool;

            /**
             * @brief VkCommandPool member.
             * 
             */
            VkCommandPool m_VKcommandPool;
    };
}
#endif // COMMANDPOOL_H
