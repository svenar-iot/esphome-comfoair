#pragma once

#include <cstdint>

namespace esphome
{
  namespace comfoair_ca
  {
    class ComfoAirComponent;

    constexpr uint8_t FAN_STATUS_PAYLOAD_SIZE{6};
    constexpr float FAN_RPM_SCALE{1875000.0F};

    bool decode_fan_status(ComfoAirComponent &component);

  } // namespace comfoair_ca
} // namespace esphome
