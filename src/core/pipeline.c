#include "pipeline.h"
#include "vulkanHelper.h"
#include "../utils.h"
#include "../render/render.h"

#include <assert.h>
#include <stddef.h>
#include <stdint.h>
#include <vulkan/vulkan_core.h>


void createDescriptorPool(RVulkanContext *vulkanContext){
  
  VkDescriptorPoolSize poolSize = {0};
  poolSize.type =  VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  poolSize.descriptorCount = MAX_FRAMES;

  VkDescriptorPoolCreateInfo poolCreateInfo = {0};
  poolCreateInfo.pNext = NULL;
  poolCreateInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
  poolCreateInfo.flags = 0;
  poolCreateInfo.poolSizeCount = 1;
  poolCreateInfo.maxSets = MAX_FRAMES;
  poolCreateInfo.pPoolSizes = &poolSize; 

  vkCreateDescriptorPool(vulkanContext->rDevice.logicalDevice, &poolCreateInfo, NULL, &vulkanContext->rPipeline.descriptorPool);

}

void createDescriptorSet(RVulkanContext *vulkanContext){
  

  VkDescriptorSetLayout setLayout[MAX_FRAMES] = {vulkanContext->rPipeline.setLayout, vulkanContext->rPipeline.setLayout};

  VkDescriptorSetAllocateInfo allocInfo = {0};
  allocInfo.pNext = NULL;
  allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
  allocInfo.descriptorPool = vulkanContext->rPipeline.descriptorPool;
  allocInfo.descriptorSetCount = 2;
  allocInfo.pSetLayouts = setLayout;

  vkAllocateDescriptorSets(vulkanContext->rDevice.logicalDevice, &allocInfo, vulkanContext->rPipeline.uniformBufferSet);

  for(size_t i = 0; i < MAX_FRAMES; ++i){
    VkDescriptorBufferInfo bufferInfo;
    bufferInfo.buffer = vulkanContext->rPipeline.uniformBuffer[i];
    bufferInfo.range = sizeof(RUniformBuffer);
    bufferInfo.offset = 0;

    VkWriteDescriptorSet write = {0};
    write.pNext = NULL;
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = vulkanContext->rPipeline.uniformBufferSet[i];
    write.dstBinding = 0;
    write.pBufferInfo = &bufferInfo;
    write.descriptorCount = 1;
    write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  
    vkUpdateDescriptorSets(vulkanContext->rDevice.logicalDevice, 1, &write, 0, NULL);
  }


}

void createRenderPass(RVulkanContext *vulkanContext){
  
  VkSubpassDependency dependencies = {0};
  dependencies.srcSubpass = VK_SUBPASS_EXTERNAL;
  dependencies.dstSubpass = 0;
  
  dependencies.srcStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;
  dependencies.dstStageMask = VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT;

  dependencies.srcAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;
  dependencies.dstAccessMask = VK_ACCESS_COLOR_ATTACHMENT_WRITE_BIT;

  VkAttachmentDescription colorDescription = {0};
  colorDescription.flags = 0;
  colorDescription.format = vulkanContext->rSwapchain.format.format;
  colorDescription.loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR;
  colorDescription.storeOp = VK_ATTACHMENT_STORE_OP_STORE;
  colorDescription.samples = VK_SAMPLE_COUNT_1_BIT;
  colorDescription.initialLayout = VK_IMAGE_LAYOUT_UNDEFINED;
  colorDescription.finalLayout = VK_IMAGE_LAYOUT_PRESENT_SRC_KHR;
  

  VkAttachmentReference colorRef = {0};
  colorRef.attachment = 0;
  colorRef.layout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL;

  VkSubpassDescription subpassDescription = {0};
  subpassDescription.pipelineBindPoint = VK_PIPELINE_BIND_POINT_GRAPHICS;
  subpassDescription.pColorAttachments = &colorRef;
  subpassDescription.colorAttachmentCount = 1;
  


  VkRenderPassCreateInfo renderPassInfo = {0};
  renderPassInfo.pNext = NULL;
  renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_CREATE_INFO;
  renderPassInfo.dependencyCount = 1;
  renderPassInfo.pDependencies = &dependencies;
  renderPassInfo.pAttachments = &colorDescription;
  renderPassInfo.attachmentCount = 1;
  renderPassInfo.pSubpasses = &subpassDescription;
  renderPassInfo.subpassCount = 1;
  
  assert(vkCreateRenderPass(vulkanContext->rDevice.logicalDevice, &renderPassInfo, NULL, &vulkanContext->rPipeline.renderPass) == VK_SUCCESS);
  

}

void createPipelineLayout(RVulkanContext *vulkanContext){


  VkDescriptorSetLayoutBinding layoutBinding = {0};
  layoutBinding.binding = 0;
  layoutBinding.stageFlags = VK_SHADER_STAGE_VERTEX_BIT;
  layoutBinding.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
  layoutBinding.descriptorCount = 1;
  layoutBinding.pImmutableSamplers = NULL;

  VkDescriptorSetLayoutCreateInfo setInfo = {0};
  setInfo.pNext = NULL;
  setInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_LAYOUT_CREATE_INFO;
  setInfo.pBindings = &layoutBinding;
  setInfo.bindingCount = 1;

  
  assert((vkCreateDescriptorSetLayout(vulkanContext->rDevice.logicalDevice, &setInfo, NULL, &vulkanContext->rPipeline.setLayout)) == VK_SUCCESS);

  VkPipelineLayoutCreateInfo layoutInfo = {0};
  layoutInfo.pNext = NULL;
  layoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
  layoutInfo.pSetLayouts = &vulkanContext->rPipeline.setLayout;
  layoutInfo.setLayoutCount = 1;

  assert(vkCreatePipelineLayout(vulkanContext->rDevice.logicalDevice, &layoutInfo, NULL, &vulkanContext->rPipeline.pipelineLayout) == VK_SUCCESS);

  
  
}

void createGraphicalPipeline(RVulkanContext *vulkanContext){
  
  uint32_t shaderCodeSize = 0;
  char *pCode = loadShader("./shaders/vert.spv", &shaderCodeSize);
 
  VkShaderModule vertexModule = {0};
  VkShaderModuleCreateInfo vertexModuleInfo = {0};
  vertexModuleInfo.pNext = NULL;
  vertexModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  vertexModuleInfo.codeSize = shaderCodeSize;
  vertexModuleInfo.pCode = (uint32_t *) pCode;
  assert(vkCreateShaderModule(vulkanContext->rDevice.logicalDevice, &vertexModuleInfo, NULL, &vertexModule ) == VK_SUCCESS);
  
  char *pCode2 = loadShader("./shaders/frag.spv", &shaderCodeSize);

  VkShaderModule fragmentModule = {0};
  VkShaderModuleCreateInfo fragmentModuleInfo = {0};
  fragmentModuleInfo.pNext = NULL;
  fragmentModuleInfo.sType = VK_STRUCTURE_TYPE_SHADER_MODULE_CREATE_INFO;
  fragmentModuleInfo.codeSize = shaderCodeSize;
  fragmentModuleInfo.pCode = (uint32_t *) pCode2;
  assert(vkCreateShaderModule(vulkanContext->rDevice.logicalDevice, &fragmentModuleInfo, NULL, &fragmentModule) == VK_SUCCESS);



  VkPipelineShaderStageCreateInfo vertexShaderStage = {0};
  vertexShaderStage.pNext = NULL;
  vertexShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  vertexShaderStage.stage = VK_SHADER_STAGE_VERTEX_BIT;
  vertexShaderStage.module = vertexModule;
  vertexShaderStage.pName = "main"; 

  VkPipelineShaderStageCreateInfo fragmentShaderStage = {0};
  fragmentShaderStage.pNext = NULL;
  fragmentShaderStage.sType = VK_STRUCTURE_TYPE_PIPELINE_SHADER_STAGE_CREATE_INFO;
  fragmentShaderStage.stage = VK_SHADER_STAGE_FRAGMENT_BIT;
  fragmentShaderStage.module = fragmentModule;
  fragmentShaderStage.pName = "main";
  

  VkPipelineShaderStageCreateInfo shaderStages[] = {fragmentShaderStage, vertexShaderStage};


  VkPipelineInputAssemblyStateCreateInfo inputInfo = {0};
  inputInfo.pNext = NULL;
  inputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_INPUT_ASSEMBLY_STATE_CREATE_INFO;
  inputInfo.topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST;
  inputInfo.primitiveRestartEnable = VK_FALSE; // READ MORE ABOUT


  VkVertexInputBindingDescription vertexBinding = {0};
  vertexBinding.binding = 0;
  vertexBinding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
  vertexBinding.stride = sizeof(RVertexData);

  VkVertexInputAttributeDescription vertexAttribute[2] = {{0}, {0}};
  vertexAttribute[0].binding = 0;
  vertexAttribute[0].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttribute[0].offset = offsetof(RVertexData, positions);
  vertexAttribute[0].location = 0;
  
  vertexAttribute[1].binding = 0;
  vertexAttribute[1].format = VK_FORMAT_R32G32B32_SFLOAT;
  vertexAttribute[1].offset = offsetof(RVertexData, colors);
  vertexAttribute[1].location = 1;



  VkPipelineVertexInputStateCreateInfo vertexInputInfo = {0};
  vertexInputInfo.pNext = NULL;
  vertexInputInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VERTEX_INPUT_STATE_CREATE_INFO;
  vertexInputInfo.vertexBindingDescriptionCount = 1;
  vertexInputInfo.vertexAttributeDescriptionCount = 2;
  vertexInputInfo.pVertexBindingDescriptions = &vertexBinding;
  vertexInputInfo.pVertexAttributeDescriptions = vertexAttribute;


  VkRect2D scissor = {{0}, {0}};
  scissor.extent =  vulkanContext->rSwapchain.extent;

  VkViewport viewport = {0};
  viewport.minDepth = 0.0f;
  viewport.maxDepth = 1.0f;
  viewport.width = vulkanContext->rSwapchain.extent.width;
  viewport.height = vulkanContext->rSwapchain.extent.height;

  VkPipelineViewportStateCreateInfo viewportInfo = {0};
  viewportInfo.pNext = NULL;
  viewportInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_VIEWPORT_STATE_CREATE_INFO;
  viewportInfo.pScissors = &scissor;
  viewportInfo.pViewports = &viewport;
  viewportInfo.scissorCount = 1;
  viewportInfo.viewportCount = 1;

  
  VkPipelineMultisampleStateCreateInfo multisampleInfo = {0};
  multisampleInfo.pNext = NULL;
  multisampleInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_MULTISAMPLE_STATE_CREATE_INFO;
  multisampleInfo.pSampleMask = NULL;
  multisampleInfo.minSampleShading = 1.0f;
  multisampleInfo.rasterizationSamples = VK_SAMPLE_COUNT_1_BIT;
  multisampleInfo.alphaToOneEnable =  VK_FALSE;
  multisampleInfo.alphaToCoverageEnable = VK_FALSE;
  multisampleInfo.sampleShadingEnable = VK_FALSE;

  VkPipelineDepthStencilStateCreateInfo depthStencilInfo = {0};
  depthStencilInfo.pNext = NULL;
  depthStencilInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_DEPTH_STENCIL_STATE_CREATE_INFO;
  depthStencilInfo.depthCompareOp = VK_COMPARE_OP_GREATER;
  depthStencilInfo.depthWriteEnable = VK_FALSE;
  depthStencilInfo.stencilTestEnable = VK_FALSE;
  depthStencilInfo.depthBoundsTestEnable = VK_FALSE;

 VkPipelineColorBlendAttachmentState colorBlendAttachment = {0};
  colorBlendAttachment.colorWriteMask = VK_COLOR_COMPONENT_R_BIT | VK_COLOR_COMPONENT_G_BIT | VK_COLOR_COMPONENT_B_BIT | VK_COLOR_COMPONENT_A_BIT;
  colorBlendAttachment.blendEnable = VK_FALSE;
  colorBlendAttachment.srcColorBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstColorBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.colorBlendOp = VK_BLEND_OP_ADD;
  colorBlendAttachment.srcAlphaBlendFactor = VK_BLEND_FACTOR_ONE;
  colorBlendAttachment.dstAlphaBlendFactor = VK_BLEND_FACTOR_ZERO;
  colorBlendAttachment.alphaBlendOp = VK_BLEND_OP_ADD;

  VkPipelineRasterizationStateCreateInfo rasterizationInfo = {0};
  rasterizationInfo.pNext = NULL;
  rasterizationInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_RASTERIZATION_STATE_CREATE_INFO;
  rasterizationInfo.cullMode = VK_CULL_MODE_NONE;
  rasterizationInfo.frontFace = VK_FRONT_FACE_CLOCKWISE;
  rasterizationInfo.lineWidth = 1.0f;
  rasterizationInfo.polygonMode = VK_POLYGON_MODE_FILL;
  rasterizationInfo.depthBiasEnable = VK_FALSE;
  rasterizationInfo.depthClampEnable = VK_FALSE;
  rasterizationInfo.rasterizerDiscardEnable = VK_FALSE;

  VkPipelineColorBlendStateCreateInfo colorBlending = {0};
  colorBlending.pNext = NULL;
  colorBlending.sType = VK_STRUCTURE_TYPE_PIPELINE_COLOR_BLEND_STATE_CREATE_INFO;
  colorBlending.logicOpEnable = VK_FALSE;
  colorBlending.logicOp = VK_LOGIC_OP_COPY;
  colorBlending.attachmentCount = 1;
  colorBlending.pAttachments = &colorBlendAttachment;
  colorBlending.blendConstants[0] = 0.0f;
  colorBlending.blendConstants[1] = 0.0f;
  colorBlending.blendConstants[2] = 0.0f;
  colorBlending.blendConstants[3] = 0.0f;  
  
  VkGraphicsPipelineCreateInfo pipelineInfo = {0};
  pipelineInfo.pNext = NULL;
  pipelineInfo.sType = VK_STRUCTURE_TYPE_GRAPHICS_PIPELINE_CREATE_INFO;
  pipelineInfo.stageCount = 2;
  pipelineInfo.pStages = shaderStages;
  pipelineInfo.layout = vulkanContext->rPipeline.pipelineLayout ; // TODO
  pipelineInfo.pVertexInputState = &vertexInputInfo;
  pipelineInfo.renderPass = vulkanContext->rPipeline.renderPass;
  pipelineInfo.subpass = 0;
  pipelineInfo.pDynamicState = NULL;
  pipelineInfo.basePipelineIndex = -1;
  pipelineInfo.basePipelineHandle = VK_NULL_HANDLE;
  pipelineInfo.pViewportState = &viewportInfo;
  pipelineInfo.pMultisampleState = &multisampleInfo;
  pipelineInfo.pDepthStencilState = &depthStencilInfo;
  pipelineInfo.pColorBlendState = &colorBlending;
  pipelineInfo.pRasterizationState = &rasterizationInfo;
  pipelineInfo.pInputAssemblyState = &inputInfo;


  assert(vkCreateGraphicsPipelines(vulkanContext->rDevice.logicalDevice, NULL,  1,&pipelineInfo, NULL, &vulkanContext->rPipeline.pipeline) == VK_SUCCESS);

  // for now
  vkDestroyShaderModule(vulkanContext->rDevice.logicalDevice, vertexModule, NULL);
  vkDestroyShaderModule(vulkanContext->rDevice.logicalDevice, fragmentModule, NULL);

  free(pCode);
  free(pCode2);

}
