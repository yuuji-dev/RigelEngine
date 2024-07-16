#include "../core/vulkanHelper.h"
#include <stdint.h>
#include <string.h>
#include "./buffer.h"
#include "../render/render.h"
#include <assert.h>
#include <vulkan/vulkan_core.h>

RVertexData vertexData[] = {
  0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
  1.0f, 0.0f, 0.0f, 0.5f, 0.5f, 0.0f,
  0.0f, -1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
  1.0f, -1.0f, 0.0f, 0.0f, 0.0f, 1.0f
};

uint32_t indexData[] = {
  0, 1, 2,
  1, 3, 2
};

void beginSingleTimeCommand(VkCommandBuffer commandBuffer){
 
  VkCommandBufferBeginInfo beginInfo = {0};
  beginInfo.pNext = NULL;
  beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
  beginInfo.flags = VK_COMMAND_BUFFER_USAGE_ONE_TIME_SUBMIT_BIT;

  vkBeginCommandBuffer(commandBuffer, &beginInfo);
}

void endSingleTimeCommand(RVulkanContext *vulkanContext,VkCommandBuffer commandBuffer, VkQueue queue, VkCommandPool commandPool){
  
  VkSubmitInfo submitInfo = {0};
  submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
  submitInfo.pCommandBuffers = &commandBuffer;
  submitInfo.commandBufferCount = 1;
  submitInfo.waitSemaphoreCount = 0;
  submitInfo.signalSemaphoreCount = 0;
  
  vkEndCommandBuffer(commandBuffer);

  vkQueueSubmit(queue, 1, &submitInfo, NULL);

  vkDeviceWaitIdle(vulkanContext->rDevice.logicalDevice);
  vkFreeCommandBuffers(vulkanContext->rDevice.logicalDevice, commandPool, 1, &commandBuffer);

  }

void copyBuffer(RVulkanContext *vulkanContext, VkBuffer srcBuffer, VkBuffer dstBuffer, VkDeviceSize size){
  
  beginSingleTimeCommand(vulkanContext->rTransfer.commandBuffer);
  
  VkBufferCopy bufferCopy = {0, 0, size};

  vkCmdCopyBuffer(vulkanContext->rTransfer.commandBuffer, srcBuffer, dstBuffer, 1, &bufferCopy);

  endSingleTimeCommand(vulkanContext, vulkanContext->rTransfer.commandBuffer, vulkanContext->rQueue.transferQueue, vulkanContext->rTransfer.commandPool);
}

int findMemoryType(VkPhysicalDeviceMemoryProperties *memProp, VkMemoryRequirements *memReq, VkMemoryPropertyFlags propFlags){

  for(int i = 0; i < memProp->memoryTypeCount; ++i){
    if( memReq->memoryTypeBits & (1 << i) && (memProp->memoryTypes[i].propertyFlags & propFlags) == propFlags){
      return i;
    }
  }

  return -1;

}

void createBuffer(RVulkanContext *vulkanContext, RCreateBufferInfo *rCreateBufferInfo){

  VkBufferCreateInfo bufferInfo = {0};
  bufferInfo.pNext = NULL;
  bufferInfo.sType = VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO;
  bufferInfo.size = rCreateBufferInfo->deviceSize;
  bufferInfo.usage = rCreateBufferInfo->usageFlags;
  bufferInfo.sharingMode = VK_SHARING_MODE_EXCLUSIVE;
  bufferInfo.queueFamilyIndexCount = 1;
  bufferInfo.pQueueFamilyIndices = &rCreateBufferInfo->queueIndex;
  
  assert(vkCreateBuffer(vulkanContext->rDevice.logicalDevice, &bufferInfo, NULL, rCreateBufferInfo->buffer) == VK_SUCCESS);
  
  
  VkMemoryRequirements memReq = {0};
  vkGetBufferMemoryRequirements(vulkanContext->rDevice.logicalDevice, *rCreateBufferInfo->buffer, &memReq);

  VkPhysicalDeviceMemoryProperties memProp = {0};
  vkGetPhysicalDeviceMemoryProperties(vulkanContext->rDevice.physicalDevice, &memProp);

  rCreateBufferInfo->memSize = memReq.size;

  VkMemoryAllocateInfo allocInfo = {0};
  allocInfo.pNext = NULL;
  allocInfo.sType = VK_STRUCTURE_TYPE_MEMORY_ALLOCATE_INFO;
  allocInfo.allocationSize = memReq.size;
  allocInfo.memoryTypeIndex = findMemoryType(&memProp, &memReq, rCreateBufferInfo->propFlag);

  assert(vkAllocateMemory(vulkanContext->rDevice.logicalDevice, &allocInfo, NULL, rCreateBufferInfo->bufferMemory) == VK_SUCCESS); 
}


void createVertexBuffer(RVulkanContext *vulkanContext){
  
  VkBuffer stagingBuffer ;
  VkDeviceMemory stagingBufferMemory;

  RCreateBufferInfo bufferInfo = {0};
  bufferInfo.buffer = &stagingBuffer;
  bufferInfo.bufferMemory = &stagingBufferMemory;
  bufferInfo.deviceSize = sizeof(vertexData);
  bufferInfo.queueIndex = vulkanContext->rQueue.transferFamilyIndex;
  bufferInfo.propFlag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  bufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;

  createBuffer(vulkanContext, &bufferInfo);

  void *data;

  vkMapMemory(vulkanContext->rDevice.logicalDevice, *bufferInfo.bufferMemory, 0, bufferInfo.memSize, 0, &data);

  memcpy(data, vertexData, sizeof(vertexData));
  vkUnmapMemory(vulkanContext->rDevice.logicalDevice, *bufferInfo.bufferMemory);

  vkBindBufferMemory(vulkanContext->rDevice.logicalDevice, *bufferInfo.buffer, *bufferInfo.bufferMemory, 0);
  
  RCreateBufferInfo realBufferInfo = bufferInfo;
  realBufferInfo.bufferMemory = &vulkanContext->rRender.vertexBufferMemory;
  realBufferInfo.buffer = &vulkanContext->rRender.vertexBuffer;
  realBufferInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
  realBufferInfo.propFlag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;

  createBuffer(vulkanContext, &realBufferInfo);
  
  vkBindBufferMemory(vulkanContext->rDevice.logicalDevice, *realBufferInfo.buffer, *realBufferInfo.bufferMemory, 0);
  copyBuffer(vulkanContext, stagingBuffer, *realBufferInfo.buffer, realBufferInfo.deviceSize);
  
  vkDestroyBuffer(vulkanContext->rDevice.logicalDevice, stagingBuffer, NULL);
  vkFreeMemory(vulkanContext->rDevice.logicalDevice, stagingBufferMemory, NULL);

}

void createUniformBuffer(RVulkanContext *vulkanContext, VkBuffer *buffer, VkDeviceMemory *bufferMemory, void **data){
  
  RCreateBufferInfo uniformInfo = {0};
  uniformInfo.deviceSize = sizeof(RUniformBuffer);
  uniformInfo.usageFlags = VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT;
  uniformInfo.propFlag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  uniformInfo.buffer = buffer;
  uniformInfo.bufferMemory = bufferMemory;
  uniformInfo.queueIndex = vulkanContext->rQueue.graphicFamilyIndex; 

  createBuffer(vulkanContext, &uniformInfo);

  vkBindBufferMemory(vulkanContext->rDevice.logicalDevice, *buffer, *bufferMemory, 0);
  vkMapMemory(vulkanContext->rDevice.logicalDevice, *bufferMemory, 0, uniformInfo.deviceSize, 0, &*data );
}

void createIndexBuffer(RVulkanContext *vulkanContext){
  
  VkBuffer stagingBuffer;
  VkDeviceMemory stagingBufferMemory;

  RCreateBufferInfo stagingInfo = {0};
  stagingInfo.deviceSize = sizeof(indexData);
  stagingInfo.buffer  = &stagingBuffer;
  stagingInfo.bufferMemory = &stagingBufferMemory;
  stagingInfo.propFlag = VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT;
  stagingInfo.usageFlags = VK_BUFFER_USAGE_TRANSFER_SRC_BIT;
  stagingInfo.queueIndex = vulkanContext->rQueue.graphicFamilyIndex;

  createBuffer(vulkanContext, &stagingInfo);

  vkBindBufferMemory(vulkanContext->rDevice.logicalDevice, stagingBuffer, stagingBufferMemory, 0);

  void *data;

  vkMapMemory(vulkanContext->rDevice.logicalDevice, stagingBufferMemory, 0, stagingInfo.deviceSize, 0, &data);

  memcpy(data, indexData, sizeof(indexData));

  RCreateBufferInfo indexInfo = {0};
  indexInfo.deviceSize = sizeof(indexData);
  indexInfo.buffer = &vulkanContext->rRender.indexBuffer;
  indexInfo.bufferMemory = &vulkanContext->rRender.indexBufferMemory;
  indexInfo.usageFlags = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
  indexInfo.propFlag = VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT;
  indexInfo.queueIndex = vulkanContext->rQueue.graphicFamilyIndex;

  createBuffer(vulkanContext, &indexInfo);

  vkBindBufferMemory(vulkanContext->rDevice.logicalDevice, *indexInfo.buffer, *indexInfo.bufferMemory, 0);
  
  copyBuffer(vulkanContext, *stagingInfo.buffer, *indexInfo.buffer, indexInfo.deviceSize);

  vulkanContext->rRender.indexCount = sizeof(indexData)  / sizeof(uint32_t);

  vkUnmapMemory(vulkanContext->rDevice.logicalDevice, *stagingInfo.bufferMemory);
  
  vkDestroyBuffer(vulkanContext->rDevice.logicalDevice, stagingBuffer, NULL);
  vkFreeMemory(vulkanContext->rDevice.logicalDevice, stagingBufferMemory, NULL);

}
