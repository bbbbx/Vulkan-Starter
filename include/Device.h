#pragma once

#include <array>
#include <vulkan/vulkan.h>
#include "QueueFlags.h"

class Instance;
class Device
{
  friend class Instance;

public:
  Instance* GetInstance();
  VkDevice GetVkDevice();
  VkQueue GetQueue(QueueFlags flag);
  unsigned int GetQueueIndex(QueueFlags flag);
  
  ~Device();

private:
  using Queues = std::array<VkQueue, sizeof(QueueFlags)>;

  Device() = delete;
  Device(Instance* instance, VkDevice vkDevice, Queues queues);

  Instance* instance;
  VkDevice vkDevice;
  Queues queues;
};


