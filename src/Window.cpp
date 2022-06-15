#include <stdexcept>

#include "Window.h"

namespace 
{
  GLFWwindow* window = nullptr;
} // namespace 

GLFWwindow* GetGLFWWindow() {
  return window;
}

void InitializeWindow(int width, int height, const char* title) {
  if (!glfwInit())
  {
    std::runtime_error("Failed to initialize glfw");
  }

  if (!glfwVulkanSupported())
  {
    std::runtime_error("Do not support Vulkan");
  }

  glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
  window = glfwCreateWindow(width, height, title, nullptr, nullptr);

  if (!window) {
    std::runtime_error("Failed to create GLFW window");
  }

}

bool ShouldQuit() {
  return !!glfwWindowShouldClose(window);
}

void DestroyWindow() {
  glfwDestroyWindow(window);
  glfwTerminate();
}
