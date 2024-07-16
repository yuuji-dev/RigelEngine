#include "app.h"
#include "./core/window.h"
#include "./inputs/keyboard.h"
#include "./core/vulkanHelper.h"
#include "./render/render.h"
#include "./resources/synchronization.h"

#include <stdint.h>
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>
#include <vulkan/vulkan_core.h>


void cleanup(RVulkanContext *vulkanContext){

  vkDestroyCommandPool(vulkanContext->rDevice.logicalDevice, vulkanContext->rRender.commandPool , NULL);
  vkDestroyCommandPool(vulkanContext->rDevice.logicalDevice, vulkanContext->rTransfer.commandPool , NULL);

  // free the frame buffer;
  
  for(size_t i = 0; i < vulkanContext->rSwapchain.imageCount ;++i){
    vkDestroyFramebuffer(vulkanContext->rDevice.logicalDevice, vulkanContext->rSwapchain.framebuffers[i], NULL);
    vkDestroyImageView(vulkanContext->rDevice.logicalDevice, vulkanContext->rSwapchain.imageViews[i], NULL);
  }

  vkDestroyBuffer(vulkanContext->rDevice.logicalDevice, vulkanContext->rRender.vertexBuffer, NULL);
  vkFreeMemory(vulkanContext->rDevice.logicalDevice, vulkanContext->rRender.vertexBufferMemory, NULL);

  for(size_t i = 0; i < MAX_FRAMES; ++i){
    vkDestroyFence(vulkanContext->rDevice.logicalDevice, vulkanContext->rDraw.inFlightFences[i], NULL);
    vkDestroySemaphore(vulkanContext->rDevice.logicalDevice, vulkanContext->rDraw.imageAvailable[i], NULL);
    vkDestroySemaphore(vulkanContext->rDevice.logicalDevice, vulkanContext->rDraw.renderFinished[i], NULL);
  }
 
  vkDestroyPipeline(vulkanContext->rDevice.logicalDevice, vulkanContext->rPipeline.pipeline, NULL);
  vkDestroyRenderPass(vulkanContext->rDevice.logicalDevice, vulkanContext->rPipeline.renderPass, NULL);
  vkDestroyPipelineLayout(vulkanContext->rDevice.logicalDevice, vulkanContext->rPipeline.pipelineLayout, NULL);
  vkDestroyDescriptorSetLayout(vulkanContext->rDevice.logicalDevice, vulkanContext->rPipeline.setLayout, NULL);
  vkDestroySwapchainKHR(vulkanContext->rDevice.logicalDevice, vulkanContext->rSwapchain.swapchain, NULL);
  vkDestroyDevice(vulkanContext->rDevice.logicalDevice, NULL);
  vkDestroySurfaceKHR(vulkanContext->rInstance.instance, vulkanContext->rWindow.surface, NULL);
  vkDestroyInstance(vulkanContext->rInstance.instance, NULL);
}

void run(){

  RVulkanContext vulkanContext;

  vulkanContext.rWindow.window = initWindow("Rigel Engine", 800, 800);

  initVulkan(&vulkanContext);

  uint32_t currentFrame = 1;
 
  vkDeviceWaitIdle(vulkanContext.rDevice.logicalDevice);
  while(!glfwWindowShouldClose(vulkanContext.rWindow.window)){
  
    processInput(vulkanContext.rWindow.window);
    
    draw(&vulkanContext, currentFrame);

    glfwPollEvents();
    currentFrame = (currentFrame + 1) %2;
  }
  

  vkDeviceWaitIdle(vulkanContext.rDevice.logicalDevice);

  cleanup(&vulkanContext);
    glfwTerminate();

}
