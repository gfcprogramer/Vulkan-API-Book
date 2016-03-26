#include "VulkanExample.hpp"

VulkanExample::VulkanExample() {
    VkResult res = initInstance();

    if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
        exitOnError("Cannot find a compatible Vulkan installable client "
            "driver (ICD). Please make sure your driver supports "
            "Vulkan before continuing. The call to vkCreateInstance failed.\n");
    }  else if (res != VK_SUCCESS) {
        exitOnError("The call to vkCreateInstance failed. Please make sure "
            "you have a Vulkan installable client driver (ICD) before "
            "continuing.\n");
    }
}

VulkanExample::~VulkanExample() {
    vkDestroyInstance(instance, NULL);
}

void VulkanExample::exitOnError(const char * msg) {
    fputs(msg, stderr);
    exit(EXIT_FAILURE);
}

VkResult VulkanExample::initInstance() {
    VkApplicationInfo appInfo{};

    memset(&appInfo, 0, sizeof(appInfo));
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = applicationName;
    appInfo.pEngineName = engineName;
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 4);

    std::vector<const char*> enabledExtensions = { VK_KHR_SURFACE_EXTENSION_NAME };

#if defined(_WIN32)
    enabledExtensions.push_back(VK_KHR_WIN32_SURFACE_EXTENSION_NAME);
#elif defined(__ANDROID__)
    enabledExtensions.push_back(VK_KHR_ANDROID_SURFACE_EXTENSION_NAME);
#elif defined(__linux__)
    enabledExtensions.push_back(VK_KHR_XCB_SURFACE_EXTENSION_NAME);
#endif

    VkInstanceCreateInfo createInfo{};

    memset(&createInfo, 0, sizeof(createInfo));
    createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
    createInfo.pNext = NULL;
    createInfo.flags = 0;
    createInfo.pApplicationInfo = &appInfo;
    createInfo.enabledExtensionCount = enabledExtensions.size();
    createInfo.ppEnabledExtensionNames = enabledExtensions.data();

    return vkCreateInstance(&createInfo, NULL, &instance);
}
