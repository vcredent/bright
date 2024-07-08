#ifndef VULKAN_CONTEXT_WIN_H
#define VULKAN_CONTEXT_WIN_H

#include "Drivers/VulkanContext.h"

class VulkanContextWin : public VulkanContext {
public:
    void CreateWindow(int width, int height);
};

#endif /* VULKAN_CONTEXT_WIN_H */
