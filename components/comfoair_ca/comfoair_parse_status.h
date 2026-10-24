#pragma once

#include <cstdint>

namespace esphome
{
  namespace comfoair_ca
  {
    class ComfoAirComponent;

    constexpr uint8_t STATUS_MIN_SIZE{11};
    constexpr uint8_t FAULTS_MIN_SIZE{9};

    constexpr uint8_t STATUS_IDX_PREHEATING{0};
    constexpr uint8_t STATUS_IDX_BYPASS{1};
    constexpr uint8_t STATUS_IDX_TYPE{2};
    constexpr uint8_t STATUS_IDX_SIZE{3};
    constexpr uint8_t STATUS_IDX_OPTIONS{4};
    constexpr uint8_t STATUS_IDX_P10_Q1{6};
    constexpr uint8_t STATUS_IDX_P18_Q1{7};
    constexpr uint8_t STATUS_IDX_P90_Q1{8};
    constexpr uint8_t STATUS_IDX_ENTHALPY{9};
    constexpr uint8_t STATUS_IDX_EWT{10};

    constexpr uint8_t STATUS_OPTION_FIREPLACE{0x01};
    constexpr uint8_t STATUS_OPTION_KITCHEN_HOOD{0x02};
    constexpr uint8_t STATUS_OPTION_POSTHEATING{0x04};
    constexpr uint8_t STATUS_OPTION_POSTHEATING_PWM{0x40};

    bool decode_status(ComfoAirComponent &component);

  } // namespace comfoair_ca
} // namespace esphome
