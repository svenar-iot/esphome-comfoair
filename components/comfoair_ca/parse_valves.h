#pragma once

#include <cstdint>

namespace esphome
{
  namespace comfoair_ca
  {
    class ComfoAirComponent;

    constexpr uint8_t VALVE_STATUS_MIN_SIZE{4};
    constexpr uint8_t BYPASS_CONTROL_MIN_SIZE{7};
    constexpr uint8_t PREHEATING_MIN_SIZE{6};

    constexpr uint8_t VALVE_IDX_BYPASS_VALVE{0};
    constexpr uint8_t VALVE_IDX_PREHEATING_STATE{1};
    constexpr uint8_t VALVE_IDX_MOTOR_CURRENT_BYPASS{2};
    constexpr uint8_t VALVE_IDX_MOTOR_CURRENT_PREHEATING{3};

    constexpr uint8_t BYPASS_IDX_FACTOR{2};
    constexpr uint8_t BYPASS_IDX_STEP{3};
    constexpr uint8_t BYPASS_IDX_CORRECTION{4};
    constexpr uint8_t BYPASS_IDX_SUMMER_MODE{6};

    constexpr uint8_t PREHEATING_IDX_VALVE{0};
    constexpr uint8_t PREHEATING_IDX_FROST_ACTIVE{1};
    constexpr uint8_t PREHEATING_IDX_STATE{2};
    constexpr uint8_t PREHEATING_IDX_FROST_MINUTES{3};
    constexpr uint8_t PREHEATING_IDX_FROST_LEVEL{5};

    bool decode_valve_status(ComfoAirComponent &component);

  } // namespace comfoair_ca
} // namespace esphome
