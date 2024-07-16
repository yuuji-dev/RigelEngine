#include "images.h"
#include "../core/vulkanHelper.h"
#include <assert.h>
#include <vulkan/vulkan_core.h>

void createImageView(RVulkanContext *vulkanContext, VkFormat format, VkImage image, VkImageAspectFlags aspect, VkImageView *imageView){
  

  VkImageViewCreateInfo imageViewInfo = {0};
  imageViewInfo.pNext = NULL;
  imageViewInfo.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
  imageViewInfo.image = image;
  imageViewInfo.format = format;
  imageViewInfo.viewType = VK_IMAGE_VIEW_TYPE_2D;
  imageViewInfo.subresourceRange.aspectMask = aspect;
  imageViewInfo.subresourceRange.layerCount = 1;
  imageViewInfo.subresourceRange.baseArrayLayer = 0;
  imageViewInfo.subresourceRange.levelCount = 1;
  imageViewInfo.subresourceRange.baseMipLevel = 0;

  assert(vkCreateImageView(vulkanContext->rDevice.logicalDevice, &imageViewInfo, NULL, imageView) == VK_SUCCESS);  
}
