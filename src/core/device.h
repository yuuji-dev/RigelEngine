#pragma once

#include <vulkan/vulkan_core.h>
#include "vulkanHelper.h"

uint32_t ratePhysicalDevice(VkPhysicalDevice physicalDevice);
void selectPhysicalDevice(RVulkanContext *vulkanContext);
VkBool32 checkPresentSupport(RVulkanContext *vulkanContext, uint32_t queueFamilyIndex);
void createLogicalDevice(RVulkanContext *vulkanContext);
void getPhysicalDeviceQueueFamilies(RVulkanContext *vulkanContext);
