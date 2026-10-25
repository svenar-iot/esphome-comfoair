#pragma once

#include <cstdint>

#ifndef COMFOAIR_HOST_TEST
#include "esphome/core/log.h"
#endif

namespace esphome
{
    namespace comfoair_ca
    {

        enum class LogLevel : uint8_t
        {
            None = 0,
            Debug = 1,
            Verbose = 2,
        };

        inline LogLevel logLevel = LogLevel::None;

    } // namespace comfoair_ca
} // namespace esphome

#define LOG_CONFIG(tag, format, ...)               \
    do                                             \
    {                                              \
        ESP_LOGCONFIG(tag, format, ##__VA_ARGS__); \
    } while (0)

#define LOG_WARN(tag, format, ...)            \
    do                                        \
    {                                         \
        ESP_LOGW(tag, format, ##__VA_ARGS__); \
    } while (0)

#define LOG_ERROR(tag, format, ...)           \
    do                                        \
    {                                         \
        ESP_LOGE(tag, format, ##__VA_ARGS__); \
    } while (0)

#define LOG_INFO(tag, format, ...)                   \
    do                                               \
    {                                                \
        if (::esphome::comfoair_ca::logLevel >=      \
            ::esphome::comfoair_ca::LogLevel::Debug) \
        {                                            \
            ESP_LOGI(tag, format, ##__VA_ARGS__);    \
        }                                            \
    } while (0)

#define LOG_DEBUG(tag, format, ...)                  \
    do                                               \
    {                                                \
        if (::esphome::comfoair_ca::logLevel >=      \
            ::esphome::comfoair_ca::LogLevel::Debug) \
        {                                            \
            ESP_LOGD(tag, format, ##__VA_ARGS__);    \
        }                                            \
    } while (0)

#define LOG_VERBOSE(tag, format, ...)                  \
    do                                                 \
    {                                                  \
        if (::esphome::comfoair_ca::logLevel >=        \
            ::esphome::comfoair_ca::LogLevel::Verbose) \
        {                                              \
            ESP_LOGD(tag, format, ##__VA_ARGS__);      \
        }                                              \
    } while (0)
