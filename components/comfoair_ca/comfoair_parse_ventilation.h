#pragma once

#include <cstdint>

#include "comfoair_commands.h"

namespace esphome
{
  namespace comfoair_ca
  {
    class ComfoAirComponent;

    constexpr uint8_t SENSOR_DATA_MIN_SIZE{10};
    constexpr uint8_t VENTILATION_LEVEL_MIN_SIZE{12};

    constexpr uint8_t VENT_IDX_EXHAUST_ABSENT{0};
    constexpr uint8_t VENT_IDX_EXHAUST_LOW{1};
    constexpr uint8_t VENT_IDX_EXHAUST_MEDIUM{2};
    constexpr uint8_t VENT_IDX_SUPPLY_ABSENT{3};
    constexpr uint8_t VENT_IDX_SUPPLY_LOW{4};
    constexpr uint8_t VENT_IDX_SUPPLY_MEDIUM{5};
    constexpr uint8_t VENT_IDX_RETURN_AIR_LEVEL{6};
    constexpr uint8_t VENT_IDX_SUPPLY_AIR_LEVEL{7};
    constexpr uint8_t VENT_IDX_CURRENT_LEVEL{8};
    constexpr uint8_t VENT_IDX_SUPPLY_FAN_ACTIVE{9};
    constexpr uint8_t VENT_IDX_EXHAUST_HIGH{10};
    constexpr uint8_t VENT_IDX_SUPPLY_HIGH{11};

    bool decode_ventilation(ComfoAirComponent &component);

  } // namespace comfoair_ca
} // namespace esphome
