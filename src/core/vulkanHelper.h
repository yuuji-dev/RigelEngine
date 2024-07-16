#pragma once

#include <cglm/types.h>
#include <stdint.h>
#include <stdbool.h>
#include <vulkan/vulkan.h>
#include <vulkan/vulkan_core.h>
#include <GLFW/glfw3.h>

#define MAX_FRAMES 2
typedef struct RUniformBuffer{
  mat4 model;
  mat4 view;
  mat4 proj;
}RUniformBuffer;

typedef struct RDraw {
  VkFence inFlightFences[MAX_FRAMES];
  VkSemaphore imageAvailable[MAX_FRAMES];
  VkSemaphore renderFinished[MAX_FRAMES];
  VkRenderPassBeginInfo renderPassBeginInfo;
  VkCommandBufferBeginInfo commandBufferBeginInfo;
  VkPresentInfoKHR presentInfo;
  VkSubmitInfo submitInfo;
}RDraw;

typedef struct RTransfer{
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer;
}RTransfer;

typedef struct RRender{
  VkCommandPool commandPool;
  VkCommandBuffer commandBuffer[MAX_FRAMES];
  VkBuffer vertexBuffer;
  VkDeviceMemory vertexBufferMemory;
  VkDeviceSize vertexBufferSize;
  void *uniformBufferMapped[MAX_FRAMES];
  VkBuffer indexBuffer;
  VkDeviceMemory indexBufferMemory;
  uint32_t indexCount;
}RRender;

typedef struct RPipeline{
  VkPipeline pipeline;
  VkPipelineLayout pipelineLayout;
  VkRenderPass renderPass;
  VkDescriptorSetLayout setLayout;
  VkDescriptorSet uniformBufferSet[MAX_FRAMES];
  VkDescriptorPool descriptorPool;
  VkBuffer uniformBuffer[MAX_FRAMES];
  VkDeviceMemory uniformBufferMemory[MAX_FRAMES];
}RPipeline;

typedef struct RSwapchain{
  VkSwapchainKHR swapchain;
  VkSurfaceFormatKHR format;
  VkSurfaceCapabilitiesKHR capablities;
  VkImage *images;
  VkImageView *imageViews;
  VkFramebuffer *framebuffers;
  uint32_t imageCount;
  VkExtent2D extent;
  VkBool32 uncapped;
}RSwapchain;

typedef struct RQueue{
  uint32_t graphicFamilyIndex;
  uint32_t presentFamilyIndex;
  uint32_t transferFamilyIndex; 
  VkQueue graphicQueue;
  VkQueue presentQueue;
  VkQueue transferQueue;
}RQueue;

typedef struct RDevice{
  VkDevice logicalDevice;
  VkPhysicalDevice physicalDevice;
  const char **enabledExtensionsNames;
  uint32_t extensionCount;
}RDevice;

typedef struct RInstance {
  VkInstance instance;
  const char* *enabledLayersNames;
  const char* *enabledExtensionsNames;
  uint32_t layerCount;
  uint32_t extensionCount;
  bool validationEnabled; 
}RInstance;

typedef struct RWindow {
  VkSurfaceKHR surface;
  GLFWwindow *window;
} RWindow;

typedef struct RVulkanContext{
  RInstance rInstance;
  RWindow rWindow;
  RDevice rDevice;
  RQueue rQueue;
  RSwapchain rSwapchain;
  RPipeline rPipeline;
  RRender rRender;
  RTransfer rTransfer;
  RDraw rDraw;
}RVulkanContext;

void initVulkan(RVulkanContext *vulkanContext);
void initRInstance(RInstance *rInstance);
void createInstance(RVulkanContext *vulkanContext);
void createWindowSurface(RVulkanContext *vulkanContext);
