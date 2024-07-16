#include "vulkanHelper.h"
#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include "../debug/logger.h"
#include <assert.h>


const char *enabledExtensionNames[] = {VK_KHR_SWAPCHAIN_EXTENSION_NAME};

uint32_t ratePhysicalDevice(VkPhysicalDevice physicalDevice){
  
  uint32_t score = 0;

  VkPhysicalDeviceProperties physicalDevicesProps = {0};
  vkGetPhysicalDeviceProperties(physicalDevice, &physicalDevicesProps);
  
  if(physicalDevicesProps.deviceType == VK_PHYSICAL_DEVICE_TYPE_DISCRETE_GPU){
    score += 100000;
  }

  score += physicalDevicesProps.limits.maxMemoryAllocationCount;

  return score;
}

void selectPhysicalDevice(RVulkanContext *vulkanContext){
  
  uint32_t physicalDevicesCount = 0;
  vkEnumeratePhysicalDevices(vulkanContext->rInstance.instance, &physicalDevicesCount, NULL);
  VkPhysicalDevice *physicalDevices = (VkPhysicalDevice *)(malloc(sizeof(VkPhysicalDevice) * physicalDevicesCount));
  vkEnumeratePhysicalDevices(vulkanContext->rInstance.instance, &physicalDevicesCount, physicalDevices);
  
  uint32_t highestScore = 0;
  uint32_t index = 0;

  for(size_t i = 0; i < physicalDevicesCount; ++i){
    uint32_t currentScore = ratePhysicalDevice(physicalDevices[i]);
  
    if(currentScore > highestScore){
      highestScore = currentScore;
      vulkanContext->rDevice.physicalDevice = physicalDevices[i];
    }

  }

  assert(highestScore != 0);  
}

VkBool32 checkPresentSupport(RVulkanContext *vulkanContext, uint32_t queueFamilyIndex){
  
  VkBool32 supported = VK_FALSE;

  vkGetPhysicalDeviceSurfaceSupportKHR(vulkanContext->rDevice.physicalDevice, queueFamilyIndex, vulkanContext->rWindow.surface, &supported);

  return supported;
}

void getPhysicalDeviceQueueFamilies(RVulkanContext *vulkanContext){
 
  uint32_t queuePropsCount = 0;
  vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext->rDevice.physicalDevice, &queuePropsCount, NULL);
  VkQueueFamilyProperties *queueProps = (VkQueueFamilyProperties *)(malloc(sizeof(VkQueueFamilyProperties) * queuePropsCount));
  vkGetPhysicalDeviceQueueFamilyProperties(vulkanContext->rDevice.physicalDevice, &queuePropsCount, queueProps);
  
  uint32_t queueFamilySelected = 0;

  for(size_t i = 0; i < queuePropsCount ;++i){
    if(queueFamilySelected == 0){
      queueFamilySelected = i;
    }

    if(queueProps[i].queueFlags & (VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_TRANSFER_BIT) && checkPresentSupport(vulkanContext, i) == VK_TRUE){
      vulkanContext->rQueue.graphicFamilyIndex = i;
      vulkanContext->rQueue.presentFamilyIndex = i;
      vulkanContext->rQueue.transferFamilyIndex = i;
      break; 
    }
    
    // TODO add support for different queues per families !VERY IMPORTANT
  }
  

}

void createLogicalDevice(RVulkanContext *vulkanContext){
  
  // TODO : rework different families
  float queuePriority = 1.0f;
  

  VkDeviceQueueCreateInfo queueInfo = {0};
  queueInfo.pNext = NULL;
  queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
  queueInfo.queueCount = 3;
  queueInfo.queueFamilyIndex = vulkanContext->rQueue.graphicFamilyIndex;
  queueInfo.flags = 0 ;
  queueInfo.pQueuePriorities = &queuePriority;
  
  VkDeviceCreateInfo deviceInfo = {0};
  deviceInfo.pNext = NULL;
  deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceInfo.flags = 0;
  deviceInfo.pQueueCreateInfos = &queueInfo;
  deviceInfo.queueCreateInfoCount = 1;
  deviceInfo.pEnabledFeatures = NULL;
  deviceInfo.enabledExtensionCount = sizeof(enabledExtensionNames)/sizeof(const char*);
  deviceInfo.ppEnabledExtensionNames = enabledExtensionNames;

  assert(vkCreateDevice(vulkanContext->rDevice.physicalDevice, &deviceInfo, NULL, &vulkanContext->rDevice.logicalDevice) == VK_SUCCESS);
  
  vkGetDeviceQueue(vulkanContext->rDevice.logicalDevice, vulkanContext->rQueue.graphicFamilyIndex , 0,  &vulkanContext->rQueue.graphicQueue);
  vkGetDeviceQueue(vulkanContext->rDevice.logicalDevice, vulkanContext->rQueue.presentFamilyIndex , 0,  &vulkanContext->rQueue.presentQueue);
  vkGetDeviceQueue(vulkanContext->rDevice.logicalDevice, vulkanContext->rQueue.transferFamilyIndex , 0,  &vulkanContext->rQueue.transferQueue);
  
}

