#include "render.h"
#include "../core/vulkanHelper.h"
#include "../resources/synchronization.h"
#include "../resources/buffer.h"
#include "../core/pipeline.h"
#include <assert.h>
#include <stdint.h>
#include <cglm/mat4.h>
#include <string.h>
#include <vulkan/vulkan_core.h>

void initRender(RVulkanContext *vulkanContext){
  createCommandPool(vulkanContext, &vulkanContext->rRender.commandPool, VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT, vulkanContext->rQueue.graphicFamilyIndex);

  for(size_t i = 0; i < MAX_FRAMES ; ++i){
    createCommandBuffer(vulkanContext, vulkanContext->rRender.commandPool, &vulkanContext->rRender.commandBuffer[i]);
  }


  createCommandPool(vulkanContext, &vulkanContext->rTransfer.commandPool, VK_COMMAND_POOL_CREATE_TRANSIENT_BIT, vulkanContext->rQueue.transferFamilyIndex);

  createCommandBuffer(vulkanContext, vulkanContext->rTransfer.commandPool, &vulkanContext->rTransfer.commandBuffer);


  // all  the render resources;

  // synchronization ;
    
  for(size_t i = 0; i < MAX_FRAMES ;++i){
    createFence(vulkanContext, VK_FENCE_CREATE_SIGNALED_BIT, &vulkanContext->rDraw.inFlightFences[i]);
    createSemaphore(vulkanContext, &vulkanContext->rDraw.imageAvailable[i]);
    createSemaphore(vulkanContext, &vulkanContext->rDraw.renderFinished[i]);
    createUniformBuffer(vulkanContext, &vulkanContext->rPipeline.uniformBuffer[i], &vulkanContext->rPipeline.uniformBufferMemory[i], &vulkanContext->rRender.uniformBufferMapped[i]);
  }

  vulkanContext->rDraw.commandBufferBeginInfo.pNext = NULL;
  vulkanContext->rDraw.commandBufferBeginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  vulkanContext->rDraw.commandBufferBeginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;
  vulkanContext->rDraw.commandBufferBeginInfo.pInheritanceInfo = NULL;

  vulkanContext->rDraw.presentInfo.pNext = NULL;
  vulkanContext->rDraw.presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
  vulkanContext->rDraw.presentInfo.pResults = NULL;
  vulkanContext->rDraw.presentInfo.pSwapchains = &vulkanContext->rSwapchain.swapchain;
  vulkanContext->rDraw.presentInfo.swapchainCount = 1;
  vulkanContext->rDraw.presentInfo.pImageIndices = 0;
  vulkanContext->rDraw.presentInfo.waitSemaphoreCount = 1;
  vulkanContext->rDraw.presentInfo.pWaitSemaphores = NULL;

  VkRect2D renderArea = {{0}, {0}};

  renderArea.extent = vulkanContext->rSwapchain.extent;

  VkClearValue *clearValues = (VkClearValue*) (malloc(sizeof(VkClearValue)));

  VkClearColorValue clear;

  clearValues->color = (VkClearColorValue){0.0f, 0.0f, 0.0f, 0.0f};

  vulkanContext->rDraw.renderPassBeginInfo.pNext = NULL;
  vulkanContext->rDraw.renderPassBeginInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
  vulkanContext->rDraw.renderPassBeginInfo.renderArea = renderArea;
  vulkanContext->rDraw.renderPassBeginInfo.clearValueCount = 1;
  vulkanContext->rDraw.renderPassBeginInfo.pClearValues = clearValues;
  vulkanContext->rDraw.renderPassBeginInfo.renderPass = vulkanContext->rPipeline.renderPass;

  vulkanContext->rDraw.submitInfo.pNext = NULL;
  vulkanContext->rDraw.submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  vulkanContext->rDraw.submitInfo.commandBufferCount = 1;
  vulkanContext->rDraw.submitInfo.waitSemaphoreCount = 1;
  vulkanContext->rDraw.submitInfo.signalSemaphoreCount = 1;

}

void createCommandPool(RVulkanContext *vulkanContext, VkCommandPool *commandPool, VkCommandPoolCreateFlags commandPoolFlags, uint32_t queueFamilyIndex){
  VkCommandPoolCreateInfo commandPoolInfo = {0};
  commandPoolInfo.pNext = NULL;
  commandPoolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
  commandPoolInfo.flags = commandPoolFlags;
  commandPoolInfo.queueFamilyIndex = queueFamilyIndex;

  assert(vkCreateCommandPool(vulkanContext->rDevice.logicalDevice, &commandPoolInfo, NULL, commandPool) == VK_SUCCESS);
}

void createCommandBuffer(RVulkanContext *vulkanContext, VkCommandPool commandPool, VkCommandBuffer *commandBuffer){
  VkCommandBufferAllocateInfo commandBufferAllocInfo = {0};
  commandBufferAllocInfo.pNext = NULL;
  commandBufferAllocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
  commandBufferAllocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
  commandBufferAllocInfo.commandBufferCount = 1;
  commandBufferAllocInfo.commandPool = commandPool;

  assert(vkAllocateCommandBuffers(vulkanContext->rDevice.logicalDevice, &commandBufferAllocInfo, commandBuffer) == VK_SUCCESS);
}

void draw(RVulkanContext *vulkanContext, uint32_t imageIndex){
  
  
  vkWaitForFences(vulkanContext->rDevice.logicalDevice, 1, &vulkanContext->rDraw.inFlightFences[imageIndex], VK_TRUE, UINT64_MAX);
 
  uint32_t currentImage = 0;


  vkAcquireNextImageKHR(vulkanContext->rDevice.logicalDevice, vulkanContext->rSwapchain.swapchain, UINT64_MAX , vulkanContext->rDraw.imageAvailable[imageIndex], VK_NULL_HANDLE, &currentImage);

  vkResetFences(vulkanContext->rDevice.logicalDevice, 1, &vulkanContext->rDraw.inFlightFences[imageIndex]);
  
  vkResetCommandBuffer(vulkanContext->rRender.commandBuffer[imageIndex], 0);

  // recording a commmand

  assert(vkBeginCommandBuffer(vulkanContext->rRender.commandBuffer[imageIndex], &vulkanContext->rDraw.commandBufferBeginInfo) == VK_SUCCESS);
  vulkanContext->rDraw.renderPassBeginInfo.framebuffer = vulkanContext->rSwapchain.framebuffers[currentImage];
  vkCmdBeginRenderPass(vulkanContext->rRender.commandBuffer[imageIndex], &vulkanContext->rDraw.renderPassBeginInfo, VK_SUBPASS_CONTENTS_INLINE);
  // geometry commands ;

  vkCmdBindPipeline(vulkanContext->rRender.commandBuffer[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext->rPipeline.pipeline);
  VkDeviceSize offset = 0;
  vkCmdBindVertexBuffers(vulkanContext->rRender.commandBuffer[imageIndex], 0, 1, &vulkanContext->rRender.vertexBuffer, &offset);
  


  vkCmdBindDescriptorSets(vulkanContext->rRender.commandBuffer[imageIndex], VK_PIPELINE_BIND_POINT_GRAPHICS, vulkanContext->rPipeline.pipelineLayout, 0, 1, &vulkanContext->rPipeline.uniformBufferSet[imageIndex], 0, 0);

  RUniformBuffer ubo = {0};

  glm_mat4_identity(ubo.model);
  glm_mat4_identity(ubo.view);
  glm_mat4_identity(ubo.proj);

  memcpy(vulkanContext->rRender.uniformBufferMapped[imageIndex], &ubo, sizeof(RUniformBuffer));
  //vkCmdDraw(vulkanContext->rRender.commandBuffer[imageIndex], 3, 1, 0, 0);
  
  vkCmdBindIndexBuffer(vulkanContext->rRender.commandBuffer[imageIndex], vulkanContext->rRender.indexBuffer, 0, VK_INDEX_TYPE_UINT32);
  vkCmdDrawIndexed(vulkanContext->rRender.commandBuffer[imageIndex], vulkanContext->rRender.indexCount, 1, 0, 0, 0);

  vkCmdEndRenderPass(vulkanContext->rRender.commandBuffer[imageIndex]);
  vkEndCommandBuffer(vulkanContext->rRender.commandBuffer[imageIndex]);
  

  vulkanContext->rDraw.submitInfo.pWaitSemaphores = &vulkanContext->rDraw.imageAvailable[imageIndex];
  
  VkPipelineStageFlags waitStages = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT; 

  vulkanContext->rDraw.submitInfo.pWaitDstStageMask = &waitStages;
  vulkanContext->rDraw.submitInfo.pCommandBuffers = &vulkanContext->rRender.commandBuffer[imageIndex];
  vulkanContext->rDraw.submitInfo.pSignalSemaphores = &vulkanContext->rDraw.renderFinished[imageIndex];

  vkQueueSubmit(vulkanContext->rQueue.graphicQueue, 1, &vulkanContext->rDraw.submitInfo, vulkanContext->rDraw.inFlightFences[imageIndex]);

  vulkanContext->rDraw.presentInfo.pWaitSemaphores = &vulkanContext->rDraw.renderFinished[imageIndex];
  vulkanContext->rDraw.presentInfo.pImageIndices = &currentImage;


  vkQueuePresentKHR(vulkanContext->rQueue.presentQueue, &vulkanContext->rDraw.presentInfo);


}
