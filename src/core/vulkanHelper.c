#include "./vulkanHelper.h"
#include <assert.h>
#include "../debug/logger.h"

#include "../resources/buffer.h"
#include "../render/render.h"
#include "device.h"
#include "framebuffer.h"
#include "pipeline.h"
#include "swapchain.h"
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>


const char *enabledLayersNames[] = {"VK_LAYER_KHRONOS_validation"};
#define LAYER_COUNT sizeof(enabledLayersNames) / sizeof(const char *)


void initVulkan(RVulkanContext *vulkanContext){
  vulkanContext->rSwapchain.uncapped = VK_TRUE;

  initRInstance(&vulkanContext->rInstance);
  createInstance(vulkanContext);
  createWindowSurface(vulkanContext);
  selectPhysicalDevice(vulkanContext);
  getPhysicalDeviceQueueFamilies(vulkanContext);
  createLogicalDevice(vulkanContext);
  createSwapchain(vulkanContext);
  createPipelineLayout(vulkanContext);
  createRenderPass(vulkanContext); 
  createGraphicalPipeline(vulkanContext);
  createFramebufferImageViews(vulkanContext);
  createFramebuffer(vulkanContext);
  createDescriptorPool(vulkanContext);
  initRender(vulkanContext);
  createDescriptorSet(vulkanContext);
  createVertexBuffer(vulkanContext);
  createIndexBuffer(vulkanContext);

}

void initRInstance(RInstance *rInstance){
  rInstance->enabledExtensionsNames = glfwGetRequiredInstanceExtensions(&rInstance->extensionCount);
  rInstance->enabledLayersNames = enabledLayersNames;

  rInstance->instance = VK_NULL_HANDLE;
  rInstance->layerCount = LAYER_COUNT;
  rInstance->validationEnabled = 0;

  uint32_t layerPropCount = 0;

  vkEnumerateInstanceLayerProperties(&layerPropCount, NULL);
  VkLayerProperties *layerProp = (VkLayerProperties*)(malloc(sizeof(VkLayerProperties) * layerPropCount));
  vkEnumerateInstanceLayerProperties(&layerPropCount, layerProp);
  
  for(size_t i = 0; i < layerPropCount ;++i){
    if(strcmp("VK_LAYER_KHRONOS_validation", layerProp[i].layerName) == 0){
      rInstance->validationEnabled = 1;
      rInstance->layerCount--;
      rInstance->enabledLayersNames += 1;
      break;
    }
  }
  
  free(layerProp);

}

void createInstance(RVulkanContext *vulkanContext){
  
  VkApplicationInfo appInfo = {0};
  appInfo.pNext = NULL;
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.apiVersion = VK_API_VERSION_1_2;
  appInfo.pEngineName = "RigelEngine";
  appInfo.engineVersion = VK_MAKE_VERSION(0, 0, 1);
  appInfo.pApplicationName = "RigelEngineApp";
  appInfo.applicationVersion = VK_MAKE_VERSION(0, 0, 1);

  VkInstanceCreateInfo createInfo = {0};
  createInfo.pNext = NULL;
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.flags = 0;
  createInfo.pApplicationInfo = &appInfo;
  createInfo.enabledLayerCount = 1;
  createInfo.ppEnabledLayerNames =  enabledLayersNames;
  createInfo.enabledExtensionCount = vulkanContext->rInstance.extensionCount;
  createInfo.ppEnabledExtensionNames = vulkanContext->rInstance.enabledExtensionsNames;
  
  assert(vkCreateInstance(&createInfo, NULL,  &vulkanContext->rInstance.instance) == VK_SUCCESS);
}

void createWindowSurface(RVulkanContext *vulkanContext){
  
  glfwCreateWindowSurface(vulkanContext->rInstance.instance, vulkanContext->rWindow.window, NULL, &vulkanContext->rWindow.surface);

}

