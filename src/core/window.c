#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>
#include <assert.h>


GLFWwindow *initWindow(const char *title, int width, int height){

  glfwInit();

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

  GLFWwindow *window = glfwCreateWindow(width, height, title, NULL, NULL);
  
  assert(window != NULL);


  return window;
}



