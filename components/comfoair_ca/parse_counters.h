#pragma once

#include <cstdint>

namespace esphome
{
  namespace comfoair_ca
  {
    class ComfoAirComponent;

    constexpr uint8_t OPERATION_HOURS_MIN_SIZE{20};
    constexpr uint8_t TIME_DELAY_MIN_SIZE{8};

    constexpr uint8_t HOURS_IDX_LEVEL0{0};
    constexpr uint8_t HOURS_IDX_LEVEL1{3};
    constexpr uint8_t HOURS_IDX_LEVEL2{6};
    constexpr uint8_t HOURS_IDX_FROST_PROTECTION{9};
    constexpr uint8_t HOURS_IDX_PREHEATING{11};
    constexpr uint8_t HOURS_IDX_BYPASS_OPEN{13};
    constexpr uint8_t HOURS_IDX_FILTER{15};
    constexpr uint8_t HOURS_IDX_LEVEL3{17};

    constexpr uint8_t DELAY_IDX_BATHROOM_ON{0};
    constexpr uint8_t DELAY_IDX_BATHROOM_OFF{1};
    constexpr uint8_t DELAY_IDX_L1_OFF{2};
    constexpr uint8_t DELAY_IDX_BOOST{3};
    constexpr uint8_t DELAY_IDX_FILTER_WARNING_WEEKS{4};
    constexpr uint8_t DELAY_IDX_RF_SHORT{5};
    constexpr uint8_t DELAY_IDX_RF_LONG{6};
    constexpr uint8_t DELAY_IDX_EXTRACTOR_HOOD_OFF{7};

    constexpr unsigned BITS_PER_BYTE{8};
    constexpr unsigned BITS_PER_WORD{16};

    bool decode_counters(ComfoAirComponent &component);

  } // namespace comfoair_ca
} // namespace esphome
