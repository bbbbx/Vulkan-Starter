#pragma once

#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

struct GLFWwindow;
GLFWwindow* GetGLFWWindow();

void InitializeWindow(int width, int height, const char* title);
bool ShouldQuit();
void DestroyWindow();
