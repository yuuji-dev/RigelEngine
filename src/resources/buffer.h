#include "../core/vulkanHelper.h"
#include <vulkan/vulkan_core.h>

typedef struct RCreateBufferInfo {
  VkDeviceSize deviceSize;
  VkBuffer *buffer;
  VkDeviceMemory *bufferMemory;
  VkBufferUsageFlags usageFlags;
  uint32_t queueIndex;
  VkMemoryPropertyFlags propFlag;
  VkDeviceSize memSize;

} RCreateBufferInfo;

void createUniformBuffer(RVulkanContext *vulkanContext, VkBuffer *buffer, VkDeviceMemory *bufferMemory, void **data);
void createBuffer(RVulkanContext *vulkanContext, RCreateBufferInfo *rCreateBufferInfo);
void createVertexBuffer(RVulkanContext *vulkanContext);
int findMemoryType(VkPhysicalDeviceMemoryProperties *memProp, VkMemoryRequirements *memReq, VkMemoryPropertyFlags propFlags);
void createIndexBuffer(RVulkanContext *vulkanContext);
