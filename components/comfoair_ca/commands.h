#pragma once

#include <cstdint>

#include "registers.h"

namespace esphome
{
  namespace comfoair_ca
  {
    enum class SimpleQuery : uint8_t
    {
      BootloaderVersion = CMD_GET_BOOTLOADER_VERSION,
      FirmwareVersion = CMD_GET_FIRMWARE_VERSION,
      ConnectorBoardVersion = CMD_GET_CONNECTOR_BOARD_VERSION,
      Status = CMD_GET_STATUS,
      FanStatus = CMD_GET_FAN_STATUS,
      ValveStatus = CMD_GET_VALVE_STATUS,
      SensorData = CMD_GET_SENSOR_DATA,
      VentilationLevel = CMD_GET_VENTILATION_LEVEL,
      Temperatures = CMD_GET_TEMPERATURES,
      Faults = CMD_GET_FAULTS,
      BypassControlStatus = CMD_GET_BYPASS_CONTROL_STATUS,
      OperationHours = CMD_GET_OPERATION_HOURS,
      PreheatingStatus = CMD_GET_PREHEATING_STATUS,
      TimeDelay = CMD_GET_TIME_DELAY,
    };

    enum class FanLevel : uint8_t
    {
      Auto = 0x00,
      Off = 0x01,
      Low = 0x02,
      Medium = 0x03,
      High = 0x04,
    };

    inline constexpr float MIN_COMFORT_TEMPERATURE{12.0F};
    inline constexpr float MAX_COMFORT_TEMPERATURE{29.0F};

    inline constexpr float COMFORT_TEMPERATURE_OFFSET{20.0F};

    inline constexpr float COMFORT_TEMPERATURE_SCALE{2.0F};

    // Wire format for temperatures: celsius = raw / 2 - 20.
    inline constexpr float TEMPERATURE_DIVISOR{2.0F};
    inline constexpr float TEMPERATURE_OFFSET{20.0F};

    inline constexpr uint8_t VENTILATION_LEVEL_PAYLOAD_SIZE{9};

    struct VentilationLevels
    {
      uint8_t exhaust_absent{0};
      uint8_t exhaust_low{0};
      uint8_t exhaust_medium{0};
      uint8_t supply_absent{0};
      uint8_t supply_low{0};
      uint8_t supply_medium{0};
      uint8_t exhaust_high{0};
      uint8_t supply_high{0};
      uint8_t unused{0};
    };

    void encode_ventilation_levels(const VentilationLevels &levels,
                                   uint8_t (&out)[VENTILATION_LEVEL_PAYLOAD_SIZE]) noexcept;

    [[nodiscard]] constexpr uint8_t encode_comfort_temperature(float temperature) noexcept
    {
      return static_cast<uint8_t>((temperature + COMFORT_TEMPERATURE_OFFSET) * COMFORT_TEMPERATURE_SCALE);
    }

    inline constexpr uint8_t ERROR_RESET_PAYLOAD[4]{1, 0, 0, 0};

    inline constexpr uint8_t FILTER_RESET_PAYLOAD[4]{0, 0, 0, 1};

  }
}
