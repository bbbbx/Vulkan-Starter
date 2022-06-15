#include <string.h>
#include <stdexcept>
#include <set>
#include <vulkan/vulkan.h>
#include "Instance.h"

#ifdef NDEBUG
const bool ENABLE_VALIDATION_LAYER = false;
#else
#include <iostream>
const bool ENABLE_VALIDATION_LAYER = true;
#endif

namespace
{
  static VKAPI_CALL VkBool32 VKAPI_CALL debugCallback(
    VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
    VkDebugUtilsMessageSeverityFlagsEXT messageType,
    const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
    void* pUserData
  ) {
    if (messageType == VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT) return VK_FALSE;

    std::cerr << "Validation layers: " << pCallbackData->pMessage << std::endl;
    return VK_FALSE;
  }

  void populateDebugMessengerCreateInfo(VkDebugUtilsMessengerCreateInfoEXT& createInfo) {
    createInfo = {};
    createInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
    createInfo.messageSeverity =
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT;
    createInfo.messageType =
      VK_DEBUG_UTILS_MESSAGE_TYPE_GENERAL_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_VALIDATION_BIT_EXT |
      VK_DEBUG_UTILS_MESSAGE_TYPE_PERFORMANCE_BIT_EXT;
    createInfo.pfnUserCallback = debugCallback;
    createInfo.pUserData = nullptr;
  }

  VkResult CreateDebugUtilsMessengerEXT(
    VkInstance instance,
    const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
    const VkAllocationCallbacks* pAllocator,
    VkDebugUtilsMessengerEXT* pDebugMessenger
  ) {
    auto func = (PFN_vkCreateDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkCreateDebugUtilsMessengerEXT");
    if (func != nullptr) {
      return func(instance, pCreateInfo, pAllocator, pDebugMessenger);
    } else {
      return VK_ERROR_EXTENSION_NOT_PRESENT;
    }
  }

  void DestroyDebugUtilsMessengerEXT(VkInstance instance, VkDebugUtilsMessengerEXT debugMessenger, const VkAllocationCallbacks* pAllocator) {
    auto func = (PFN_vkDestroyDebugUtilsMessengerEXT)vkGetInstanceProcAddr(instance, "vkDestroyDebugUtilsMessengerEXT");
    if (func != nullptr) {
      func(instance, debugMessenger, pAllocator);
    }
  }

  std::vector<const char*> validationLayers = {
    // "VK_LAYER_standard_validation",
    "VK_LAYER_KHRONOS_validation"
  };

  bool checkValidationLayerSupport() {
    // Get all available layers
    uint32_t layerCount = 0;
    vkEnumerateInstanceLayerProperties(&layerCount, nullptr);

    std::vector<VkLayerProperties> availableLayers(layerCount);
    vkEnumerateInstanceLayerProperties(&layerCount, availableLayers.data());

    // 
    for (const auto* requiredLayerName : validationLayers)
    {
      bool layerFound = false;

      for (const auto& layer : availableLayers)
      {
        if (strcmp(layer.layerName, requiredLayerName) == 0) {
          layerFound = true;
          break;
        }
      }
      
      if (!layerFound) {
        return false;
      }
    }

    return true;
  }

  /**
   * @brief Get the Required Extensions object
   *        根据 ENABLE_VALIDATION_LAYER 变量决定是否使用
   *        VK_EXT_DEBUG_UTILS_EXTENSION_NAME device extension
   * @return std::vector<const char*> 
   */
  std::vector<const char*> getRequiredExtensions() {
    std::vector<const char*> extensions;

    if (ENABLE_VALIDATION_LAYER) {
      extensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
    }

    return extensions;
  }

  QueueFamilyIndices checkDeviceQueueSupport(
    VkPhysicalDevice device,
    QueueFlagBits requiredQueues,
    VkSurfaceKHR surface = VK_NULL_HANDLE
  ) {
    uint32_t queueFamilyCount = 0;
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);

    std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
    vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());

    VkQueueFlags requiredVulkanQueues = 0;
    if (requiredQueues[QueueFlags::Graphics]) {
      requiredVulkanQueues |= VK_QUEUE_GRAPHICS_BIT;
    }
    if (requiredQueues[QueueFlags::Compute]) {
      requiredVulkanQueues |= VK_QUEUE_COMPUTE_BIT;
    }
    if (requiredQueues[QueueFlags::Transfer]) {
      requiredVulkanQueues |= VK_QUEUE_TRANSFER_BIT;
    }

    QueueFamilyIndices indices = {};
    indices.fill(-1);
    VkQueueFlags supportedQueues = 0;
    bool needsPresent = requiredQueues[QueueFlags::Present];
    bool presentSupported = false;

    int i = 0;
    for (const auto& queueFamily : queueFamilies) {
      if (queueFamily.queueCount > 0) {
        supportedQueues |= queueFamily.queueFlags;
      }

      if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_GRAPHICS_BIT) {
        indices[QueueFlags::Graphics] = i;
      }

      if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_COMPUTE_BIT) {
        indices[QueueFlags::Compute] = i;
      }

      if (queueFamily.queueCount > 0 && queueFamily.queueFlags & VK_QUEUE_TRANSFER_BIT) {
        indices[QueueFlags::Transfer] = i;
      }

      if (needsPresent) {
        VkBool32 presentSupport = false;
        vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
        if (queueFamily.queueCount > 0 && presentSupport) {
          presentSupported = true;
          indices[QueueFlags::Present] = i;
        }
      }

      if ((requiredVulkanQueues & supportedQueues) == requiredVulkanQueues && (!needsPresent || presentSupported)) {
        break;
      }

      i++;
    }

    return indices;
  }

  /**
   * @brief Check the physical device for specified extension support
   * 
   * @param device 
   * @param requiredExtensions 
   * @return true 
   * @return false 
   */
  bool checkDeviceExtensionSupport(VkPhysicalDevice device, std::vector<const char*> requiredExtensions) {
    uint32_t extensionCount;
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);

    std::vector<VkExtensionProperties> availableExtensions(extensionCount);
    vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());

    std::set<std::string> requiredExtensionSet(requiredExtensions.begin(), requiredExtensions.end());

    for (const auto& extension : availableExtensions)
    {
      requiredExtensionSet.erase(extension.extensionName);
    }

    return requiredExtensionSet.empty();
  }
} // namespace 


Instance::Instance(const char* applicationName, unsigned int additionalExtensionCount, const char** additionalExtensions)
{
  // --- Specify details about our application ---
  VkApplicationInfo appInfo = {};
  appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
  appInfo.apiVersion = VK_API_VERSION_1_0;
  appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
  appInfo.pApplicationName = applicationName;
  appInfo.pEngineName = "No Engine";

  // --- Create Vulkan instance ---
  VkInstanceCreateInfo createInfo = {};
  createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
  createInfo.pApplicationInfo = &appInfo;

  // Get extensions necessary for Vulkan to interface with GLFW
  std::vector<const char*> extensions = getRequiredExtensions();
  for (unsigned int i = 0; i < additionalExtensionCount; ++i)
  {
    extensions.push_back(additionalExtensions[i]);
  }
  createInfo.enabledExtensionCount = static_cast<uint32_t>(extensions.size());
  createInfo.ppEnabledExtensionNames = extensions.data();

  // Specify global validation layers
  if (ENABLE_VALIDATION_LAYER) {
    if (!checkValidationLayerSupport()) {
      throw std::runtime_error("Validation layers requested, but not supoorted");
    }

    createInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    createInfo.ppEnabledLayerNames = validationLayers.data();

    VkDebugUtilsMessengerCreateInfoEXT debugCreateInfo = {};
    populateDebugMessengerCreateInfo(debugCreateInfo);
    createInfo.pNext = (VkDebugUtilsMessengerCreateInfoEXT*)(&debugCreateInfo);
  } else {
    createInfo.enabledLayerCount = 0;
  }

  if (vkCreateInstance(&createInfo, VK_NULL_HANDLE, &instance) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create vulkan instance");
  }

  // Set up debug utils messenger
  setupDebugMessenger();
}

void Instance::setupDebugMessenger() {
  if (!ENABLE_VALIDATION_LAYER) return;

  VkDebugUtilsMessengerCreateInfoEXT createInfo = {};
  populateDebugMessengerCreateInfo(createInfo);

  if (CreateDebugUtilsMessengerEXT(instance, &createInfo, nullptr, &debugMessenger) != VK_SUCCESS) {
    throw std::runtime_error("Failed to set up debug messenger");
  }
}



Instance::~Instance()
{
  if (ENABLE_VALIDATION_LAYER) {
    DestroyDebugUtilsMessengerEXT(instance, debugMessenger, nullptr);
  }

  vkDestroyInstance(instance, nullptr);
}


void Instance::PickPhysicalDevice(
  std::vector<const char*> deviceExtensions,
  QueueFlagBits requiredQueues,
  VkSurfaceKHR surface
) {
  // List the graphics cards on the machine
  uint32_t deviceCount = 0;
  vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);

  if (deviceCount == 0) {
    throw std::runtime_error("Failed to find GPUs with Vulkan support");
  }

  std::vector<VkPhysicalDevice> devices(deviceCount);
  vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

  // Evaluate each GPU and check if it is suitable
  for (const auto& device : devices) {
    bool queueSupport = true;
    queueFamilyIndices = checkDeviceQueueSupport(device, requiredQueues, surface);
    for (unsigned int i = 0; i < requiredQueues.size(); ++i) {
      if (requiredQueues[i]) {
        queueSupport &= (queueFamilyIndices[i] >= 0);
      }
    }

    if (requiredQueues[QueueFlags::Present]) {
      // Get basic surface capabilities
      vkGetPhysicalDeviceSurfaceCapabilitiesKHR(device, surface, &surfaceCapabilities);

      // Query supported surface formats
      uint32_t formatCount;
      vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);

      if (formatCount != 0) {
        surfaceFormats.resize(formatCount);
        vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, surfaceFormats.data());
      }

      // Query supported presentation modes
      uint32_t presentModeCount;
      vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);

      if (presentModeCount != 0) {
        presentModes.resize(presentModeCount);
        vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, presentModes.data());
      }
    }

    if (queueSupport &&
      checkDeviceExtensionSupport(device, deviceExtensions) &&
      (!requiredQueues[QueueFlags::Present] || (!surfaceFormats.empty() && ! presentModes.empty()))
    ) {
      physicalDevice = device;
      break;
    }
  }

  this->deviceExtensions = deviceExtensions;

  if (physicalDevice == VK_NULL_HANDLE) {
    throw std::runtime_error("Failed to find a suitable GPU");
  }

  vkGetPhysicalDeviceMemoryProperties(physicalDevice, &deviceMemoryProperties);
}

Device* Instance::CreateDevice(QueueFlagBits requiredQueues, VkPhysicalDeviceFeatures deviceFeatures) {
  std::set<int> uniqueQueueFamilies;
  bool queueSupport = true;
  for (unsigned int i = 0; i < requiredQueues.size(); i++)
  {
    if (requiredQueues[i]) {
      queueSupport &= (queueFamilyIndices[i] >=0);
      uniqueQueueFamilies.insert(queueFamilyIndices[i]);
    }
  }

  if (!queueSupport) {
    throw std::runtime_error("Device does not support requested queues");
  }

  std::vector<VkDeviceQueueCreateInfo> queueCreateInfos;
  float queuePriority = 1.0f;
  for (int queueFamilyIndex : uniqueQueueFamilies)
  {
    VkDeviceQueueCreateInfo queueCreateInfo = {};
    queueCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueCreateInfo.queueFamilyIndex = queueFamilyIndex;
    queueCreateInfo.queueCount = 1;
    queueCreateInfo.pQueuePriorities = &queuePriority;

    queueCreateInfos.push_back(queueCreateInfo);
  }

  // --- Create logical device ---
  VkDeviceCreateInfo deviceCreateInfo = {};
  deviceCreateInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
  deviceCreateInfo.queueCreateInfoCount = static_cast<uint32_t>(queueCreateInfos.size());
  deviceCreateInfo.pQueueCreateInfos = queueCreateInfos.data();

  deviceCreateInfo.pEnabledFeatures = &deviceFeatures;

  // Enable device-specific extensions and validation layers
  deviceCreateInfo.enabledExtensionCount = static_cast<uint32_t>(deviceExtensions.size());
  deviceCreateInfo.ppEnabledExtensionNames = deviceExtensions.data();

  if (ENABLE_VALIDATION_LAYER) {
    deviceCreateInfo.enabledLayerCount = static_cast<uint32_t>(validationLayers.size());
    deviceCreateInfo.ppEnabledLayerNames = validationLayers.data();
  } else {
    deviceCreateInfo.enabledLayerCount = 0;
  }

  VkDevice vkDevice;
  if (vkCreateDevice(physicalDevice, &deviceCreateInfo, nullptr, &vkDevice) != VK_SUCCESS) {
    throw std::runtime_error("Failed to create logical device");
  }

  Device::Queues queues;
  for (unsigned int i = 0; i < requiredQueues.size(); i++)
  {
    if (requiredQueues[i]) {
      vkGetDeviceQueue(vkDevice, queueFamilyIndices[i], 0, &queues[i]);
    }
  }

  return new Device(this, vkDevice, queues);
}
