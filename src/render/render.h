#pragma once

#include <cglm/types.h>
#include <stdint.h>
#include "../core/vulkanHelper.h"
#include <cglm/vec3.h>

typedef struct RVertexData {
  vec3 positions;
  vec3 colors;
} RVertexData;

void createCommandPool(RVulkanContext *vulkanContext, VkCommandPool *commandPool, VkCommandPoolCreateFlags commandPoolFlags, uint32_t queueFamilyIndex);

void createCommandBuffer(RVulkanContext *vulkanContext, VkCommandPool commandPool, VkCommandBuffer *commandBuffer);

void draw(RVulkanContext *vulkanContext, uint32_t imageIndex);

void initRender(RVulkanContext *vulkanContext);
