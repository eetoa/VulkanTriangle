#include "stdafx.h"


const uint32_t WIDTH = 800;
const uint32_t HEIGHT = 600;

class VulkanHelloWorld {

public:

    void Run() {
        initWindow();
        initVulkan();
        mainLoop();
        cleanup();
    }

private:

    void initWindow() {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);
        window = glfwCreateWindow(WIDTH, HEIGHT, "VulkanHelloWorld", nullptr, nullptr);
    }

    void initVulkan() {
        createInstance();
        createSurface();
        pickPhysicalDevice();
    }

    void mainLoop() {
        while (!glfwWindowShouldClose(window)) {
            glfwPollEvents();
        }
    }

    void cleanup() {
        vkDestroySurfaceKHR(instance, surface, nullptr);
        vkDestroyInstance(instance, nullptr);

        glfwDestroyWindow(window);
        glfwTerminate();
    }

    void createInstance() {
        // 应用程序信息
        VkApplicationInfo appInfo{};            // 结构体
        appInfo.sType = VK_STRUCTURE_TYPE_APPLICATION_INFO;
        appInfo.pApplicationName = "Hello Triangle";
        appInfo.applicationVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.pEngineName = "No Engine";
        appInfo.engineVersion = VK_MAKE_VERSION(1, 0, 0);
        appInfo.apiVersion = VK_API_VERSION_1_0;

        // 实例创建信息
        VkInstanceCreateInfo createInfo = {};   // 结构体
        createInfo.sType = VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;
        createInfo.pApplicationInfo = &appInfo;
        // 扩展 - 和窗体系统交互
        uint32_t glfwExtensionCount = 0;
        const char** glfwExtensions;
        glfwExtensions = glfwGetRequiredInstanceExtensions(&glfwExtensionCount);
        createInfo.enabledExtensionCount = glfwExtensionCount;
        createInfo.ppEnabledExtensionNames = glfwExtensions;
        // 全局校验层
        createInfo.enabledLayerCount = 0;

        // 创建实例
        // 实例创建信息
        if (vkCreateInstance(&createInfo, nullptr, &instance) != VK_SUCCESS)
            throw std::runtime_error("Failed to create instance!");
    }

    void createSurface() {
        if (glfwCreateWindowSurface(instance, window, nullptr, &surface) != VK_SUCCESS)
            throw std::runtime_error("Failed to create window surface!");
    }

    void pickPhysicalDevice() {
        // 获取设备数量
        uint32_t deviceCount = 0;
        vkEnumeratePhysicalDevices(instance, &deviceCount, nullptr);
        if (deviceCount == 0)
            throw std::runtime_error("Failed to find GPUs with Vulkan support!");

        // 获取所有可用的设备
        std::vector<VkPhysicalDevice> devices(deviceCount);
        vkEnumeratePhysicalDevices(instance, &deviceCount, devices.data());

        // 选取一个合适的设备
        for (const auto& device : devices) {
            // 查询物理设备属性
            VkPhysicalDeviceProperties deviceProperties;
            vkGetPhysicalDeviceProperties(device, &deviceProperties);
            // 选择集显
            bool integratedGraphicsCard = deviceProperties.deviceType == VK_PHYSICAL_DEVICE_TYPE_INTEGRATED_GPU;
            // 查询合适的队列族
            std::optional<uint32_t> queueFamily = findQueueFamily(device);
            bool queueFamilySupported = queueFamily.has_value();
            // 查询交换链支持
            std::string swapchainExtensionName = VK_KHR_SWAPCHAIN_EXTENSION_NAME;
            uint32_t extensionCount;
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, nullptr);
            std::vector<VkExtensionProperties> availableExtensions(extensionCount);
            vkEnumerateDeviceExtensionProperties(device, nullptr, &extensionCount, availableExtensions.data());
            bool swapchainExtensionSupported = false;
            for (auto properties : availableExtensions)
                if (properties.extensionName == swapchainExtensionName)
                    swapchainExtensionSupported = true;
            bool swapchainSupported = false;
            if (swapchainExtensionSupported) {
                uint32_t formatCount;
                uint32_t presentModeCount;
                vkGetPhysicalDeviceSurfaceFormatsKHR(device, surface, &formatCount, nullptr);
                vkGetPhysicalDeviceSurfacePresentModesKHR(device, surface, &presentModeCount, nullptr);
                swapchainSupported = formatCount && presentModeCount;
            }
            // 选择具有合适的队列族且支持交换链的集显
            if (integratedGraphicsCard && queueFamilySupported && swapchainSupported) {
                physicalDevice = device;
                break;
            }
        }
        if (physicalDevice == VK_NULL_HANDLE)
            throw std::runtime_error("Failed to find a suitable GPU!");
    }

    // 获取一个同时支持 图形命令 且支持我们的 窗体表面 的队列族
    // 注意到uint32_t的任何取值都有可能是一个真实存在的队列族索引
    // 因此我们使用std::optional来处理获取失败的情况
    std::optional<uint32_t> findQueueFamily(VkPhysicalDevice device) {
        // 获取所有的队列族
        uint32_t queueFamilyCount = 0;
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, nullptr);
        std::vector<VkQueueFamilyProperties> queueFamilies(queueFamilyCount);
        vkGetPhysicalDeviceQueueFamilyProperties(device, &queueFamilyCount, queueFamilies.data());
        // 选取队列族
        for (int i = 0; i < queueFamilies.size(); i++) {
            // 支持表面
            VkBool32 presentSupport = false;
            vkGetPhysicalDeviceSurfaceSupportKHR(device, i, surface, &presentSupport);
            // 支持图形命令
            VkBool32 graphicsSupport = queueFamilies[i].queueFlags & VK_QUEUE_GRAPHICS_BIT;
            if (graphicsSupport && presentSupport)
                return i;
        }
        return std::optional<uint32_t>();
    }



private:
    GLFWwindow* window;

    // Vulkan实例
    VkInstance instance;

    // 窗体表面实例
    VkSurfaceKHR surface;

    // 物理设备对象
    VkPhysicalDevice physicalDevice = VK_NULL_HANDLE;
};


int main() {
    VulkanHelloWorld helloWorld;
    helloWorld.Run();
    return 0;
}
