#pragma once

#include <cstdint>

#include "comfoair_commands.h"

namespace esphome
{
  namespace comfoair_ca
  {
    class ComfoAirComponent;

    constexpr uint8_t TEMPERATURE_STATUS_MIN_SIZE{4};
    constexpr uint8_t TEMPERATURES_MIN_SIZE{9};

    constexpr uint8_t TEMPERATURES_IDX_PRESENCE{5};
    constexpr uint8_t TEMPERATURES_IDX_EWT{6};
    constexpr uint8_t TEMPERATURES_IDX_REHEATING{7};
    constexpr uint8_t TEMPERATURES_IDX_KITCHEN_HOOD{8};

    constexpr uint8_t PRESENT_OUTSIDE_AIR{0x01};
    constexpr uint8_t PRESENT_SUPPLY_AIR{0x02};
    constexpr uint8_t PRESENT_RETURN_AIR{0x04};
    constexpr uint8_t PRESENT_EXHAUST_AIR{0x08};
    constexpr uint8_t PRESENT_EWT{0x10};
    constexpr uint8_t PRESENT_REHEATING{0x20};
    constexpr uint8_t PRESENT_KITCHEN_HOOD{0x40};

    bool decode_temperatures(ComfoAirComponent &component);

  } // namespace comfoair_ca
} // namespace esphome
