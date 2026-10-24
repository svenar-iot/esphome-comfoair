#pragma once

#include "esphome/core/log.h"

namespace esphome
{
  namespace comfoair_ca
  {
    inline bool logEnable = false;
  } // namespace comfoair_ca
} // namespace esphome

#define LOG_INFO(tag, format, ...)                \
    do                                            \
    {                                             \
        if (::esphome::comfoair_ca::logEnable)    \
        {                                         \
            ESP_LOGI(tag, format, ##__VA_ARGS__); \
        }                                         \
    } while (0)

#define LOG_CONFIG(tag, format, ...)                   \
    do                                                 \
    {                                                  \
        if (::esphome::comfoair_ca::logEnable)         \
        {                                              \
            ESP_LOGCONFIG(tag, format, ##__VA_ARGS__); \
        }                                              \
    } while (0)

#define LOG_WARN(tag, format, ...)                \
    do                                            \
    {                                             \
        ESP_LOGW(tag, format, ##__VA_ARGS__);     \
    } while (0)

#define LOG_ERROR(tag, format, ...)               \
    do                                            \
    {                                             \
        if (::esphome::comfoair_ca::logEnable)    \
        {                                         \
            ESP_LOGE(tag, format, ##__VA_ARGS__); \
        }                                         \
    } while (0)

#define LOG_DEBUG(tag, format, ...)               \
    do                                            \
    {                                             \
        if (::esphome::comfoair_ca::logEnable)    \
        {                                         \
            ESP_LOGD(tag, format, ##__VA_ARGS__); \
        }                                         \
    } while (0)

#define LOG_VERBOSE(tag, format, ...)             \
    do                                            \
    {                                             \
        if (::esphome::comfoair_ca::logEnable)    \
        {                                         \
            ESP_LOGV(tag, format, ##__VA_ARGS__); \
        }                                         \
    } while (0)

#define LOG_VERY_VERBOSE(tag, format, ...)         \
    do                                             \
    {                                              \
        if (::esphome::comfoair_ca::logEnable)     \
        {                                          \
            ESP_LOGVV(tag, format, ##__VA_ARGS__); \
        }                                          \
    } while (0)
