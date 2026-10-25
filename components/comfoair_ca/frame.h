#pragma once

#include <cstddef>
#include <cstdint>

#include "checksum.h"

namespace esphome
{
  namespace comfoair_ca
  {
    enum class ByteVerdict : uint8_t
    {
      Valid,
      Complete,
      BadChecksum,
      Invalid,
    };

    struct ByteCheck
    {
      ByteVerdict verdict{ByteVerdict::Valid};
      uint8_t expected_checksum{0};
      uint8_t declared_length{0};
    };

    [[nodiscard]] ByteCheck check_frame_byte(const uint8_t *buffer, std::size_t index) noexcept;

    [[nodiscard]] std::size_t declared_frame_length(const uint8_t *buffer) noexcept;

  } // namespace comfoair_ca
} // namespace esphome
