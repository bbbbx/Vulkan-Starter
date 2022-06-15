#pragma once

#include <bitset>
#include <vector>
#include <vulkan/vulkan.h>
#include "QueueFlags.h"
#include "Device.h"

extern const bool ENABLE_VALIDATION_LAYER;

class Instance
{
private:
  /* data */
  VkInstance instance;
  VkDebugUtilsMessengerEXT debugMessenger;

  QueueFamilyIndices queueFamilyIndices;
  VkSurfaceCapabilitiesKHR surfaceCapabilities;
  std::vector<VkSurfaceFormatKHR> surfaceFormats;
  std::vector<VkPresentModeKHR> presentModes;
  VkPhysicalDeviceMemoryProperties deviceMemoryProperties;

  VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
  std::vector<const char*> deviceExtensions;

  void setupDebugMessenger();
public:
  Instance() = delete;
  Instance(const char* applicationName, unsigned int additionalExtensionCount, const char** additionalExtensions);
  ~Instance();

  VkInstance GetVkInstance() { return instance; }

  void PickPhysicalDevice(std::vector<const char*> deviceExtensions, QueueFlagBits requiredQueues, VkSurfaceKHR surface);

  Device* CreateDevice(QueueFlagBits requiredQueues, VkPhysicalDeviceFeatures deviceFeatures);

  const QueueFamilyIndices& GetQueueFamilyIndices() const { return queueFamilyIndices; }
};


