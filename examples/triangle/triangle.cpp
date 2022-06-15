#include <iostream>

#include "Window.h"
#include "Instance.h"
#include "QueueFlags.h"

int main(int argc, char const *argv[])
{
  int width = 800;
  int height = 800;
  const char* applicationName = "fuck";
  InitializeWindow(width, height, applicationName);

  uint32_t glfwExtensionCount = 0;
  const char** glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
  Instance* instance = new Instance(applicationName, glfwExtensionCount, glfwExtensions);

  VkSurfaceKHR surface;
  if (glfwCreateWindowSurface(instance->GetVkInstance(), GetGLFWWindow(), nullptr, &surface) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create window surface");
  }

  instance->PickPhysicalDevice(
    { VK_KHR_SWAPCHAIN_EXTENSION_NAME },
    QueueFlagBit::ComputeBit | QueueFlagBit::GraphicsBit | QueueFlagBit::PresentBit | QueueFlagBit::TransferBit,
    surface
  );

  while (!ShouldQuit()) {
    glfwPollEvents();
  }

  return 0;
}
