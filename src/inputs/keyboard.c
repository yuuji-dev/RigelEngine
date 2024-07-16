#include <GLFW/glfw3.h>



void inputCallback(GLFWwindow *window, int keycode, int scancode, int action, int mods){
    
  if(keycode == GLFW_KEY_ESCAPE && action == GLFW_PRESS){
    glfwSetWindowShouldClose(window, GLFW_TRUE);
  }

}

void processInput(GLFWwindow *window){
 glfwSetKeyCallback(window, inputCallback);
}

