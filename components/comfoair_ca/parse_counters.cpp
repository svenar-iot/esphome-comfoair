// 0xDA mixes 24-bit and 16-bit big-endian counters; 0xDE is eight single-byte
// minute values.

#include "parse_counters.h"

#include "comfoair.h"

namespace esphome
{
  namespace comfoair_ca
  {
    bool decode_counters(ComfoAirComponent &component)
    {
      const uint8_t command{component.data_[COMMAND_IDX_MSG_ID]};
      const uint8_t length{component.payload_length()};

      if (command == RES_GET_OPERATION_HOURS)
      {
        if (length < OPERATION_HOURS_MIN_SIZE)
        {
          LOG_WARN(TAG, "Operation hours: expected %i bytes, got %i",
                   static_cast<int>(OPERATION_HOURS_MIN_SIZE), static_cast<int>(length));
          return false;
        }

        const auto read_u24 = [&component](uint8_t index) noexcept -> uint32_t
        {
          const uint32_t high{component.payload_byte(index)};
          const uint32_t middle{component.payload_byte(static_cast<uint8_t>(index + 1))};
          const uint32_t low{component.payload_byte(static_cast<uint8_t>(index + 2))};
          return (high << BITS_PER_WORD) | (middle << BITS_PER_BYTE) | low;
        };

        const auto read_u16 = [&component](uint8_t index) noexcept -> uint32_t
        {
          const uint32_t high{component.payload_byte(index)};
          const uint32_t low{component.payload_byte(static_cast<uint8_t>(index + 1))};
          return (high << BITS_PER_BYTE) | low;
        };

        if (component.level0_hours != nullptr)
        {
          component.level0_hours->publish_state(static_cast<float>(read_u24(HOURS_IDX_LEVEL0)));
        }
        if (component.level1_hours != nullptr)
        {
          component.level1_hours->publish_state(static_cast<float>(read_u24(HOURS_IDX_LEVEL1)));
        }
        if (component.level2_hours != nullptr)
        {
          component.level2_hours->publish_state(static_cast<float>(read_u24(HOURS_IDX_LEVEL2)));
        }
        if (component.level3_hours != nullptr)
        {
          component.level3_hours->publish_state(static_cast<float>(read_u24(HOURS_IDX_LEVEL3)));
        }
        if (component.frost_protection_hours != nullptr)
        {
          component.frost_protection_hours->publish_state(
              static_cast<float>(read_u16(HOURS_IDX_FROST_PROTECTION)));
        }
        if (component.preheating_hours != nullptr)
        {
          component.preheating_hours->publish_state(static_cast<float>(read_u16(HOURS_IDX_PREHEATING)));
        }
        if (component.bypass_open_hours != nullptr)
        {
          component.bypass_open_hours->publish_state(static_cast<float>(read_u16(HOURS_IDX_BYPASS_OPEN)));
        }
        if (component.filter_hours != nullptr)
        {
          component.filter_hours->publish_state(static_cast<float>(read_u16(HOURS_IDX_FILTER)));
        }
        return true;
      }

      if (command == RES_GET_TIME_DELAY)
      {
        if (length < TIME_DELAY_MIN_SIZE)
        {
          LOG_WARN(TAG, "Time delay: expected %i bytes, got %i",
                   static_cast<int>(TIME_DELAY_MIN_SIZE), static_cast<int>(length));
          return false;
        }

        if (component.bathroom_switch_on_delay_minutes != nullptr)
        {
          component.bathroom_switch_on_delay_minutes->publish_state(
              component.payload_byte(DELAY_IDX_BATHROOM_ON));
        }
        if (component.bathroom_switch_off_delay_minutes != nullptr)
        {
          component.bathroom_switch_off_delay_minutes->publish_state(
              component.payload_byte(DELAY_IDX_BATHROOM_OFF));
        }
        if (component.l1_switch_off_delay_minutes != nullptr)
        {
          component.l1_switch_off_delay_minutes->publish_state(component.payload_byte(DELAY_IDX_L1_OFF));
        }
        if (component.boost_ventilation_minutes != nullptr)
        {
          component.boost_ventilation_minutes->publish_state(component.payload_byte(DELAY_IDX_BOOST));
        }
        if (component.filter_warning_weeks != nullptr)
        {
          component.filter_warning_weeks->publish_state(component.payload_byte(DELAY_IDX_FILTER_WARNING_WEEKS));
        }
        if (component.rf_high_time_short_minutes != nullptr)
        {
          component.rf_high_time_short_minutes->publish_state(component.payload_byte(DELAY_IDX_RF_SHORT));
        }
        if (component.rf_high_time_long_minutes != nullptr)
        {
          component.rf_high_time_long_minutes->publish_state(component.payload_byte(DELAY_IDX_RF_LONG));
        }
        if (component.extractor_hood_switch_off_delay_minutes != nullptr)
        {
          component.extractor_hood_switch_off_delay_minutes->publish_state(
              component.payload_byte(DELAY_IDX_EXTRACTOR_HOOD_OFF));
        }
        return true;
      }

      return false;
    }

  }
}
