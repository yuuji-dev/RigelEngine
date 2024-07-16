#pragma once

#include <vulkan/vulkan_core.h>
#include "../core/vulkanHelper.h"

void createImageView(RVulkanContext *vulkanContext, VkFormat format, VkImage image, VkImageAspectFlags aspect, VkImageView *imageView);
