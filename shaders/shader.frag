#version 430

layout(location = 0) in vec3 inColors;
layout(location = 0) out vec4 fragColor;

void main(){
  fragColor = vec4(inColors, 1.0f);
}
