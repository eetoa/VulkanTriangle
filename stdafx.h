#pragma once

// system
#include <Windows.h>
#include <cstdint>
#include <stdexcept>
#include <vector>
#include <optional>
#include <math.h>

// Graphic
#include <vulkan/vulkan.h>
#include <GLFW/glfw3.h>

// Lib
#ifndef _WIN64
#pragma comment(lib,"glfw\\lib-vc2010-32\\glfw3.lib")
#else
#pragma comment(lib,"glfw\\lib-vc2010-64\\glfw3.lib")
#endif

#pragma comment(lib, "Legacy\\lib\\legacy_stdio_definitions.lib")

#pragma comment(lib, "vulkan\\Lib\\vulkan-1.lib")