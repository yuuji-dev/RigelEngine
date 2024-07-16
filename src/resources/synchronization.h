#pragma once

#include "../core/vulkanHelper.h"


void createFence(RVulkanContext *vulkanContext, VkFenceCreateFlags flags, VkFence *fence);
void createSemaphore(RVulkanContext *vulkanContext,VkSemaphore *semaphore);
