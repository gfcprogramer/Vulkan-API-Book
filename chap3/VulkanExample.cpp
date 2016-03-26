#include "VulkanExample.hpp"

VulkanExample::VulkanExample()
{
    initInstance();
    initDevices();
}

VulkanExample::~VulkanExample()
{
    vkDestroyInstance(instance, NULL);
}

void VulkanExample::exitOnError(const char * msg)
{
    fputs(msg, stderr);
    exit(EXIT_FAILURE);
}

void VulkanExample::initInstance()
{
    VkApplicationInfo appInfo{};

    memset(&appInfo, 0, sizeof(appInfo));
    appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
    appInfo.pNext = NULL;
    appInfo.pApplicationName = applicationName;
    appInfo.pEngineName = engineName;
    appInfo.apiVersion = VK_MAKE_VERSION(1, 0, 3);

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

    VkResult res = vkCreateInstance(&createInfo, NULL, &instance);

    if (res == VK_ERROR_INCOMPATIBLE_DRIVER) {
        exitOnError("Cannot find a compatible Vulkan installable client "
            "driver (ICD). Please make sure your driver supports "
            "Vulkan before continuing. The call to vkCreateInstance failed.");
    } else if (res != VK_SUCCESS) {
        exitOnError("The call to vkCreateInstance failed. Please make sure "
            "you have a Vulkan installable client driver (ICD) before "
            "continuing.");
    }
}

void VulkanExample::initDevices()
{
    uint32_t deviceCount = 0;
    VkResult result = vkEnumeratePhysicalDevices(instance, &deviceCount, NULL);

    if (result != VK_SUCCESS)
        exitOnError ("Failed to enumerate physical devices in the system.");

    if (deviceCount < 1) {
        exitOnError ("vkEnumeratePhysicalDevices did not report any availible "
            "devices that support Vulkan. Do you have a compatible Vulkan "
            "installable client driver (ICD)?");
    }

    std::vector<VkPhysicalDevice> physicalDevices(deviceCount);
    result = vkEnumeratePhysicalDevices(instance, &deviceCount, physicalDevices.data());

    if (result != VK_SUCCESS)
        exitOnError ("Failed to enumerate physical devices in the system.");

    physicalDevice = physicalDevices[0];

    float priorities[] = { 1.0f };
    VkDeviceQueueCreateInfo queueInfo{};
    queueInfo.sType = VK_STRUCTURE_TYPE_DEVICE_QUEUE_CREATE_INFO;
    queueInfo.pNext = NULL;
    queueInfo.flags = 0;
    queueInfo.queueFamilyIndex = 0;
    queueInfo.queueCount = 1;
    queueInfo.pQueuePriorities = &priorities[0];

    std::vector<const char *> enabledExtensions = { VK_KHR_SWAPCHAIN_EXTENSION_NAME };
    VkDeviceCreateInfo deviceInfo{};
    deviceInfo.sType = VK_STRUCTURE_TYPE_DEVICE_CREATE_INFO;
    deviceInfo.pNext = NULL;
    deviceInfo.flags = 0;
    deviceInfo.queueCreateInfoCount = 1;
    deviceInfo.pQueueCreateInfos = &queueInfo;
    deviceInfo.enabledExtensionCount = enabledExtensions.size();
    deviceInfo.ppEnabledExtensionNames = enabledExtensions.data();
    deviceInfo.pEnabledFeatures = NULL;

    result = vkCreateDevice(physicalDevice, &deviceInfo, NULL, &device);

    if (result != VK_SUCCESS)
        exitOnError ("Failed to create a Vulkan logical device.");

    VkPhysicalDeviceProperties physicalProperties;

    for (uint32_t i = 0; i < deviceCount; i++) {
        vkGetPhysicalDeviceProperties(physicalDevices[i], &physicalProperties);
        fprintf(stdout, "Device Name:    %s\n", physicalProperties.deviceName);
        fprintf(stdout, "Device Type:    %d\n", physicalProperties.deviceType);
        fprintf(stdout, "Driver Version: %d\n", physicalProperties.driverVersion);
        fprintf(stdout, "API Version:    %d.%d.%d\n",
            VK_VERSION_MAJOR(physicalProperties.apiVersion),
            VK_VERSION_MINOR(physicalProperties.apiVersion),
            VK_VERSION_PATCH(physicalProperties.apiVersion));
    }
}
