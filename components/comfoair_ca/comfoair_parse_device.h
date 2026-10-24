#pragma once

#include <cstddef>
#include <cstdint>

namespace esphome
{
  namespace comfoair_ca
  {
    class ComfoAirComponent;

    constexpr uint8_t VERSION_PAYLOAD_SIZE{13};
    constexpr uint8_t CONNECTOR_PAYLOAD_SIZE{14};
    constexpr std::size_t DESCRIPTION_BUFFER_SIZE{64};

    bool decode_device_info(ComfoAirComponent &component);

  } // namespace comfoair_ca
} // namespace esphome
