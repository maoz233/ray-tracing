/**
 * @file config.h
 * @author Mao Zhang (mao.zhang233@gmail.com)
 * @brief
 * @version 1.0
 * @date 2023-03-19
 *
 * @copyright Copyright (c) 2023
 *
 */
#ifndef RAY_TRACING_INCLUDE_CONFIG_H_
#define RAY_TRACING_INCLUDE_CONFIG_H_

#include <limits>

namespace rt {

#ifdef _DEBUG
#define RAY_TRACING_ENABLE_VULKAN_VALIDATION_LAYER
#endif

const int WIDTH = 1500;
const int HEIGHT = 762;
const char* const TITLE = "Ray Tracing";

#ifdef _WIN32
const char* const FONTS_FILEPATH{"../../fonts/Roboto-Medium.ttf"};
#else
const char* const FONTS_FILEPATH{"../fonts/Roboto-Medium.ttf"};
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

const float INFINITY_F = std::numeric_limits<float>::infinity();
const float PI = 3.1415926f;

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_CONFIG_H_
