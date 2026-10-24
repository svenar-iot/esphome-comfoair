#include "comfoair_checksum.h"

namespace esphome
{
  namespace comfoair_ca
  {
    uint8_t checksum_bytes(const uint8_t *data, std::size_t length) noexcept
    {
      uint16_t sum{CHECKSUM_SEED};
      for (std::size_t i{0}; i < length; ++i)
      {
        if (data == nullptr)
        {
          break;
        }
        sum = static_cast<uint16_t>(sum + data[i]);
      }
      return static_cast<uint8_t>(sum & 0xFFU);
    }

    uint8_t checksum_body(const uint8_t *body, std::size_t payload_length) noexcept
    {
      if (body == nullptr)
      {
        return CHECKSUM_SEED;
      }
      const std::size_t span{payload_length + 3U};
      if (span < payload_length)
      {
        return CHECKSUM_SEED;
      }
      return checksum_bytes(body, span);
    }

    uint8_t append_escaped(uint8_t *out, uint8_t index, uint8_t value) noexcept
    {
      if (out == nullptr)
      {
        return index;
      }
      out[index] = value;
      ++index;
      if (value == ESCAPE_BYTE)
      {
        out[index] = ESCAPE_BYTE;
        ++index;
      }
      return index;
    }

    bool index_is_escaped_region(std::size_t index, std::size_t payload_length) noexcept
    {
      const std::size_t first_escaped{2};
      const std::size_t last_escaped{FRAME_HEADER_LENGTH + payload_length};
      if (last_escaped < FRAME_HEADER_LENGTH)
      {
        return false;
      }
      return index >= first_escaped && index <= last_escaped;
    }

  }
}
