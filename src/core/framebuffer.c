
#include "./framebuffer.h"
#include "./vulkanHelper.h"

#include <assert.h>
#include <vulkan/vulkan_core.h>
#include <stdlib.h>
#include "../resources/images.h"



void createFramebufferImageViews(RVulkanContext *vulkanContext){
  
  vulkanContext->rSwapchain.imageViews = (VkImageView*)(malloc(sizeof(VkImageView) * vulkanContext->rSwapchain.imageCount)); 

  for(size_t i = 0; i < vulkanContext->rSwapchain.imageCount; ++i){
    createImageView(vulkanContext, vulkanContext->rSwapchain.format.format, vulkanContext->rSwapchain.images[i], VK_IMAGE_ASPECT_COLOR_BIT, &vulkanContext->rSwapchain.imageViews[i]);
  }

}

void createFramebuffer(RVulkanContext *vulkanContext){

  vulkanContext->rSwapchain.framebuffers = (VkFramebuffer *)(malloc(sizeof(VkFramebuffer) * vulkanContext->rSwapchain.imageCount));

  for(size_t i = 0; i < vulkanContext->rSwapchain.imageCount ;++i){
    VkFramebufferCreateInfo framebufferInfo = {0};
    framebufferInfo.pNext = NULL;
    framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
    framebufferInfo.width = vulkanContext->rSwapchain.extent.width;
    framebufferInfo.height = vulkanContext->rSwapchain.extent.height;
    framebufferInfo.layers = 1;
    framebufferInfo.renderPass = vulkanContext->rPipeline.renderPass;
    framebufferInfo.pAttachments = &vulkanContext->rSwapchain.imageViews[i];
    framebufferInfo.attachmentCount = 1;
  
    assert(vkCreateFramebuffer(vulkanContext->rDevice.logicalDevice, &framebufferInfo, NULL, &vulkanContext->rSwapchain.framebuffers[i]) == VK_SUCCESS);
  } 

}
