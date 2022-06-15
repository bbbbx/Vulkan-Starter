#include "Device.h"
#include "Instance.h"

Device::Device(Instance* instance, VkDevice vkDevice, Queues queues)
  : instance(instance), vkDevice(vkDevice), queues(queues)
{
}

Device::~Device() {
  vkDestroyDevice(vkDevice, nullptr);
}

Instance* Device::GetInstance() {
  return instance;
}

VkDevice Device::GetVkDevice() {
  return vkDevice;
}

VkQueue Device::GetQueue(QueueFlags flag) {
  return queues[flag];
}

unsigned int Device::GetQueueIndex(QueueFlags flag) {
  return GetInstance()->GetQueueFamilyIndices()[flag];
}
