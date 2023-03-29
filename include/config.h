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

namespace rt {

#ifdef _DEBUG
#define RAY_TRACING_ENABLE_VULKAN_VALIDATION_LAYER
#endif

const int WIDTH = 1366;
const int HEIGHT = 768;
const char* const TITLE = "Ray Tracing";

#ifdef _WIN32
const char* const FONTS_FILEPATH{"../../fonts/Roboto-Medium.ttf"};
#else
const char* const FONTS_FILEPATH{"../fonts/Roboto-Medium.ttf"};
#endif

const int MAX_FRAMES_IN_FLIGHT = 2;

}  // namespace rt

#endif  // RAY_TRACING_INCLUDE_CONFIG_H_
