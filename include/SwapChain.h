#pragma once

#include <vector>
#include "Device.h"

class Device;
class SwapChain
{
  friend class Device;

public:
  ~SwapChain();

private:
  SwapChain(Device* device, VkSurfaceKHR vkSurface, unsigned int numBuffers);
  void Create();
  void Destroy();
  VkSwapchainKHR GetVkSwapChain() const;
  VkFormat GetVkImageFormat() const;
  VkExtent2D GetVkExtent() const;
  uint32_t GetIndex() const;

  Device* device;
  VkSurfaceKHR vkSurface;
  unsigned int numBuffers;
  VkSwapchainKHR vkSwapChain;

  std::vector<VkImage> vkSwapChainImages;

  VkFormat vkSwapChainImageFormat;
  VkExtent2D vkSwapChainExtent;

  VkSemaphore imageAvailableSemaphore;
  VkSemaphore renderFinishedSemaphore;
};

