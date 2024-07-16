
#include <vulkan/vulkan_core.h>
#include <assert.h>
#include "../core/vulkanHelper.h"

void createSemaphore(RVulkanContext *vulkanContext,VkSemaphore *semaphore){
  VkSemaphoreCreateInfo semaphoreCreateInfo = {0};
  semaphoreCreateInfo.pNext = NULL;
  semaphoreCreateInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;
  
  assert(vkCreateSemaphore(vulkanContext->rDevice.logicalDevice, &semaphoreCreateInfo, NULL, semaphore) == VK_SUCCESS);
}

void createFence(RVulkanContext *vulkanContext, VkFenceCreateFlags flags, VkFence *fence){

  VkFenceCreateInfo fenceCreateInfo = {0};
  fenceCreateInfo.pNext = NULL;
  fenceCreateInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
  fenceCreateInfo.flags = flags;

  assert(vkCreateFence(vulkanContext->rDevice.logicalDevice, &fenceCreateInfo, NULL, fence) == VK_SUCCESS);
  
}
