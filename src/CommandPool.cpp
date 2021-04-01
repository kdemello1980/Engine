#include "../include/CommandPool.h"
#include "../include/LogicalDevice.h"

#include <vulkan/vulkan.h>
#include <iostream>

namespace KMDM
{
    /**
     * @brief Initialize static CommandPool pointer.
     * 
     */
    CommandPool* CommandPool::m_commandPool = nullptr;

    /**
     * @brief 
     * 
     * @return CommandPool* 
     */
    CommandPool* CommandPool::getInstance()
    {
        if (!m_commandPool)
        {
            m_commandPool = new CommandPool();
        }
        return m_commandPool;
    }

    /**
     * @brief 
     * 
     * @return VkCommandPool 
     */
    VkCommandPool CommandPool::getCommandPool()
    {
        return m_commandPool->m_VKcommandPool;
    }

    /**
     * @brief Construct a new Command Pool:: Command Pool object
     * 
     */
    CommandPool::CommandPool()
    {
        VkCommandPoolCreateInfo poolInfo = {};
        poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
        poolInfo.queueFamilyIndex = LogicalDevice::getInstance()->getQueueFamilyInfo().graphicsFamilyIndex.value();

        // Allow resetting of individual command buffers.
        poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;

        if (vkCreateCommandPool(LogicalDevice::getInstance()->getLogicalDevice(), &poolInfo, nullptr, &m_VKcommandPool)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to create command pool.");
        }
        std::cout << "Created command pool." << std::endl;
    }

    /**
     * @brief 
     * 
     */
    void CommandPool::destroyCommandPool()
    {
        std::cout << "Destroying CommandPool." << std::endl;
        vkDestroyCommandPool(LogicalDevice::getInstance()->getLogicalDevice(), m_VKcommandPool, nullptr);
        m_commandPool = nullptr;
    }

    /**
     * @brief Destroy the Command Pool:: Command Pool object
     * 
     */
    CommandPool::~CommandPool()
    {
        destroyCommandPool();
    }

    /**
     * @brief 
     * 
     * @return VkCommandBuffer 
     */
    VkCommandBuffer CommandPool::beginSingleTimeCommands()
    {
        // Command buffer allocation info.
        VkCommandBufferAllocateInfo allocInfo = {};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = m_VKcommandPool;
        allocInfo.commandBufferCount = 1;

        // Allocate command buffer.
        VkCommandBuffer buffer;
        if (vkAllocateCommandBuffers(LogicalDevice::getInstance()->getLogicalDevice(), &allocInfo, &buffer)
            != VK_SUCCESS)
        {
            throw std::runtime_error("Failed to allocate command buffer.");
        }

        // Begin command buffer.
        VkCommandBufferBeginInfo beginInfo = {};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
        beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

        vkBeginCommandBuffer(buffer, &beginInfo);
        return buffer;
    }


    /**
     * @brief 
     * 
     * @param buffer 
     */
    void CommandPool::endSingleTimeCommands(VkCommandBuffer buffer)
    {
        // End the buffer.
        vkEndCommandBuffer(buffer);

        // Submit the buffer.
        VkSubmitInfo submitInfo = {};
        submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
        submitInfo.commandBufferCount = 1;
        submitInfo.pCommandBuffers = &buffer;

        vkQueueSubmit(LogicalDevice::getInstance()->getGraphicsQueue(), 1, &submitInfo, VK_NULL_HANDLE);
        vkQueueWaitIdle(LogicalDevice::getInstance()->getGraphicsQueue());

        vkFreeCommandBuffers(LogicalDevice::getInstance()->getLogicalDevice(),
            m_VKcommandPool, 1, &buffer);
    } /// endSingleTimeCommands

}
