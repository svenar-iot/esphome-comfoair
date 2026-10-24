#include "comfoair_frame.h"

namespace esphome
{
  namespace comfoair_ca
  {
    namespace
    {
      constexpr std::size_t LEN_INDEX{COMMAND_IDX_DATA};

      constexpr std::size_t MAX_PAYLOAD{
          COMMAND_MAX_LENGTH - FRAME_HEADER_LENGTH - FRAME_TRAILER_LENGTH};

      [[nodiscard]] constexpr bool payload_fits(std::size_t payload_length) noexcept
      {
        return payload_length <= MAX_PAYLOAD;
      }

    }

    std::size_t declared_frame_length(const uint8_t *buffer) noexcept
    {
      if (buffer == nullptr)
      {
        return 0;
      }
      return frame_length(buffer[LEN_INDEX]);
    }

    ByteCheck check_frame_byte(const uint8_t *buffer, std::size_t index) noexcept
    {
      ByteCheck result{};
      if (buffer == nullptr)
      {
        result.verdict = ByteVerdict::Invalid;
        return result;
      }

      const uint8_t byte{buffer[index]};

      switch (index)
      {
      case 0:
        result.verdict = (byte == FRAME_PREFIX) ? ByteVerdict::Valid : ByteVerdict::Invalid;
        return result;

      case 1:
        if (byte == FRAME_ACK)
        {
          result.verdict = ByteVerdict::Complete;
          return result;
        }
        result.verdict = (byte == FRAME_HEAD) ? ByteVerdict::Valid : ByteVerdict::Invalid;
        return result;

      case 2:
        result.verdict = (byte == 0x00) ? ByteVerdict::Valid : ByteVerdict::Invalid;
        return result;

      default:
        break;
      }

      const std::size_t payload_length{buffer[LEN_INDEX]};
      result.declared_length = static_cast<uint8_t>(payload_length);

      if (index < FRAME_HEADER_LENGTH)
      {
        result.verdict = ByteVerdict::Valid;
        return result;
      }

      if (!payload_fits(payload_length))
      {
        result.verdict = ByteVerdict::Invalid;
        return result;
      }

      const std::size_t checksum_index{FRAME_HEADER_LENGTH + payload_length};

      if (index < checksum_index)
      {
        result.verdict = ByteVerdict::Valid;
        return result;
      }

      if (index == checksum_index)
      {
        const uint8_t expected{checksum_body(buffer + 2, payload_length)};
        result.expected_checksum = expected;
        result.verdict = (byte == expected) ? ByteVerdict::Valid : ByteVerdict::BadChecksum;
        return result;
      }

      if (index == checksum_index + 1)
      {
        result.verdict = (byte == FRAME_PREFIX) ? ByteVerdict::Valid : ByteVerdict::Invalid;
        return result;
      }

      if (index == checksum_index + 2)
      {
        result.verdict = (byte == FRAME_TAIL) ? ByteVerdict::Complete : ByteVerdict::Invalid;
        return result;
      }

      result.verdict = ByteVerdict::Invalid;
      return result;
    }

  }
}
