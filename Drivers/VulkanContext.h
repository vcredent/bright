#ifndef VULKAN_CONTEXT_H
#define VULKAN_CONTEXT_H

#define GLFWwindow unsigned *

class VulkanContext {
protected:
    void _CreateWindow(int width, int height, GLFWwindow *window);
};

#endif /* VULKAN_CONTEXT_H */
