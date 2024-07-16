#include "./vulkanHelper.h"
#include <GLFW/glfw3.h>
#include <stdint.h>
#include <stdlib.h>
#include <assert.h>
#include "swapchain.h"
#include <vulkan/vulkan_core.h>
#include "../utils.h"

void getSupportedSurfaceFormat(RVulkanContext *vulkanContext){
  
  uint32_t formatCount = 0;
  vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext->rDevice.physicalDevice, vulkanContext->rWindow.surface, &formatCount, NULL);
  VkSurfaceFormatKHR *formats = (VkSurfaceFormatKHR *)(malloc(sizeof(VkSurfaceFormatKHR) * formatCount));
  vkGetPhysicalDeviceSurfaceFormatsKHR(vulkanContext->rDevice.physicalDevice, vulkanContext->rWindow.surface, &formatCount, formats);

  for(size_t i = 0; i < formatCount; ++i){
    if(formats[i].colorSpace == VK_COLOR_SPACE_SRGB_NONLINEAR_KHR && (formats[i].format == VK_FORMAT_R8G8B8A8_SRGB || formats[i].format == VK_FORMAT_B8G8R8A8_SRGB)  ){
      vulkanContext->rSwapchain.format = formats[i]; 
      break;
    }
  }
  
  free(formats);

};

void getWindowExtent(RVulkanContext *vulkanContext){

  VkSurfaceCapabilitiesKHR *surfaceCapablities = &vulkanContext->rSwapchain.capablities;
  
  if(surfaceCapablities->currentExtent.width != UINT32_MAX){
    vulkanContext->rSwapchain.extent = surfaceCapablities->currentExtent;
  }else{
    VkExtent2D extent = {0};

    glfwGetFramebufferSize(vulkanContext->rWindow.window, (int*) &extent.width, (int*) &extent.height);
    
    vulkanContext->rSwapchain.extent.width = clamp(extent.width, surfaceCapablities->minImageExtent.width, surfaceCapablities->maxImageExtent.width);
    vulkanContext->rSwapchain.extent.height = clamp(extent.height, surfaceCapablities->minImageExtent.height, surfaceCapablities->maxImageExtent.height);
  }

}

VkPresentModeKHR selectSwapchainPresentMode(RVulkanContext *vulkanContext){


  uint32_t presentModeCount = 0;
  vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanContext->rDevice.physicalDevice, vulkanContext->rWindow.surface, &presentModeCount, NULL);
  
  VkPresentModeKHR *presentModes = (VkPresentModeKHR *) (malloc(sizeof(VkPresentModeKHR) * presentModeCount));
  vkGetPhysicalDeviceSurfacePresentModesKHR(vulkanContext->rDevice.physicalDevice, vulkanContext->rWindow.surface, &presentModeCount, presentModes);

  VkPresentModeKHR presentMode = VK_PRESENT_MODE_FIFO_KHR;
  
  if(vulkanContext->rSwapchain.uncapped == VK_TRUE){
    return VK_PRESENT_MODE_IMMEDIATE_KHR;
  }


  for(size_t i = 0; i < presentModeCount ;++i){
    if(presentModes[i] == VK_PRESENT_MODE_MAILBOX_KHR){

      presentMode = presentModes[i];
      break;
    }
  }
  
  free(presentModes);

  return presentMode;

}

void getSurfaceCapabilities(RVulkanContext *vulkanContext){
  vkGetPhysicalDeviceSurfaceCapabilitiesKHR(vulkanContext->rDevice.physicalDevice, vulkanContext->rWindow.surface, &vulkanContext->rSwapchain.capablities);
}

void createSwapchain(RVulkanContext *vulkanContext){


  getSupportedSurfaceFormat(vulkanContext);
  
  getSurfaceCapabilities(vulkanContext);

  getWindowExtent(vulkanContext);


  VkSwapchainCreateInfoKHR swapchainInfo = {0};
  swapchainInfo.pNext = NULL;
  swapchainInfo.sType = VK_STRUCTURE_TYPE_SWAPCHAIN_CREATE_INFO_KHR;
  swapchainInfo.surface = vulkanContext->rWindow.surface;
  swapchainInfo.clipped = VK_FALSE;
  swapchainInfo.queueFamilyIndexCount = 1;
  swapchainInfo.pQueueFamilyIndices = &vulkanContext->rQueue.presentFamilyIndex;
  swapchainInfo.presentMode = selectSwapchainPresentMode(vulkanContext);
  swapchainInfo.imageFormat = vulkanContext->rSwapchain.format.format;
  swapchainInfo.imageColorSpace = vulkanContext->rSwapchain.format.colorSpace;
  swapchainInfo.imageSharingMode = VK_SHARING_MODE_EXCLUSIVE;
  swapchainInfo.compositeAlpha = VK_COMPOSITE_ALPHA_OPAQUE_BIT_KHR;
  swapchainInfo.oldSwapchain = VK_NULL_HANDLE;
  swapchainInfo.minImageCount = vulkanContext->rSwapchain.capablities.minImageCount + 1;
  swapchainInfo.imageUsage = VK_IMAGE_USAGE_COLOR_ATTACHMENT_BIT;
  swapchainInfo.preTransform = VK_SURFACE_TRANSFORM_IDENTITY_BIT_KHR;
  swapchainInfo.imageExtent = vulkanContext->rSwapchain.extent;
  swapchainInfo.imageArrayLayers = 1;


  assert(vkCreateSwapchainKHR(vulkanContext->rDevice.logicalDevice, &swapchainInfo, NULL, &vulkanContext->rSwapchain.swapchain) == VK_SUCCESS);
  
   vkGetSwapchainImagesKHR(vulkanContext->rDevice.logicalDevice, vulkanContext->rSwapchain.swapchain, &vulkanContext->rSwapchain.imageCount, NULL); 
  
  vulkanContext->rSwapchain.images = (VkImage *)(malloc(sizeof(VkImage) * vulkanContext->rSwapchain.imageCount));

  vkGetSwapchainImagesKHR(vulkanContext->rDevice.logicalDevice, vulkanContext->rSwapchain.swapchain, &vulkanContext->rSwapchain.imageCount, vulkanContext->rSwapchain.images);

  


}
