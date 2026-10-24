#pragma once

#include <cstddef>
#include <cstdint>

#include "registers.h"

namespace esphome
{
  namespace comfoair_ca
  {
    inline constexpr uint8_t CHECKSUM_SEED{173};
    inline constexpr std::size_t FRAME_HEADER_LENGTH{COMMAND_LEN_HEAD};
    inline constexpr std::size_t FRAME_TRAILER_LENGTH{COMMAND_LEN_TAIL};
    inline constexpr uint8_t FRAME_PREFIX{COMMAND_PREFIX};
    inline constexpr uint8_t FRAME_HEAD{COMMAND_HEAD};
    inline constexpr uint8_t FRAME_TAIL{COMMAND_TAIL};
    inline constexpr uint8_t FRAME_ACK{COMMAND_ACK};
    inline constexpr uint8_t ESCAPE_BYTE{COMMAND_PREFIX};

    [[nodiscard]] uint8_t checksum_bytes(const uint8_t *data, std::size_t length) noexcept;

    [[nodiscard]] uint8_t checksum_body(const uint8_t *body, std::size_t payload_length) noexcept;

    uint8_t append_escaped(uint8_t *out, uint8_t index, uint8_t value) noexcept;

    [[nodiscard]] bool index_is_escaped_region(std::size_t index, std::size_t payload_length) noexcept;

    [[nodiscard]] constexpr std::size_t frame_length(std::size_t payload_length) noexcept
    {
      return FRAME_HEADER_LENGTH + payload_length + FRAME_TRAILER_LENGTH;
    }

    [[nodiscard]] constexpr std::size_t max_escaped_frame_length(std::size_t payload_length) noexcept
    {
      return FRAME_HEADER_LENGTH + 2U * (payload_length + 3U) + FRAME_TRAILER_LENGTH;
    }

  } // namespace comfoair_ca
} // namespace esphome
