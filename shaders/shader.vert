#version 430

layout(binding = 0) uniform RUniformBuffer {
  mat4 model;
  mat4 view;
  mat4 proj;
} ubo;

layout(location = 0) in vec3 positions;
layout(location = 1) in vec3 colors;

layout(location = 0) out vec3 outColor;

void main(){
  outColor = colors;
  gl_Position =  ubo.model * vec4(positions, 1.0);
}

