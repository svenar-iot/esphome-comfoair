// 0xD2 gates each temperature behind a presence bitmask at byte 5, so a
// sensor that is not fitted is skipped rather than reported as -20 C.

#include "parse_temperatures.h"

#include "comfoair.h"

namespace esphome
{
  namespace comfoair_ca
  {
    namespace
    {
      [[nodiscard]] constexpr float decode_temperature(uint8_t raw) noexcept
      {
        return (static_cast<float>(raw) / TEMPERATURE_DIVISOR) - TEMPERATURE_OFFSET;
      }
    }

    bool decode_temperatures(ComfoAirComponent &component)
    {
      const uint8_t command{component.data_[COMMAND_IDX_MSG_ID]};
      const uint8_t length{component.payload_length()};

      if (command == RES_GET_TEMPERATURE_STATUS)
      {
        if (length < TEMPERATURE_STATUS_MIN_SIZE)
        {
          LOG_WARN(TAG, "Temperature status: expected %i bytes, got %i",
                   static_cast<int>(TEMPERATURE_STATUS_MIN_SIZE), static_cast<int>(length));
          return false;
        }
        if (component.outside_air_temperature != nullptr)
        {
          component.outside_air_temperature->publish_state(decode_temperature(component.payload_byte(0)));
        }
        if (component.supply_air_temperature != nullptr)
        {
          component.supply_air_temperature->publish_state(decode_temperature(component.payload_byte(1)));
        }
        if (component.return_air_temperature != nullptr)
        {
          component.return_air_temperature->publish_state(decode_temperature(component.payload_byte(2)));
        }
        if (component.exhaust_air_temperature != nullptr)
        {
          component.exhaust_air_temperature->publish_state(decode_temperature(component.payload_byte(3)));
        }
        return true;
      }

      if (command == RES_GET_TEMPERATURES)
      {
        if (length < TEMPERATURES_MIN_SIZE)
        {
          LOG_WARN(TAG, "Temperatures: expected %i bytes, got %i",
                   static_cast<int>(TEMPERATURES_MIN_SIZE), static_cast<int>(length));
          return false;
        }

        component.target_temperature = decode_temperature(component.payload_byte(0));
        component.publish_state();

        const uint8_t present{component.payload_byte(TEMPERATURES_IDX_PRESENCE)};

        if (component.outside_air_temperature != nullptr && (present & PRESENT_OUTSIDE_AIR) != 0)
        {
          component.outside_air_temperature->publish_state(decode_temperature(component.payload_byte(1)));
        }
        if (component.supply_air_temperature != nullptr && (present & PRESENT_SUPPLY_AIR) != 0)
        {
          component.supply_air_temperature->publish_state(decode_temperature(component.payload_byte(2)));
        }
        if ((present & PRESENT_RETURN_AIR) != 0)
        {
          const float return_air{decode_temperature(component.payload_byte(3))};
          if (component.return_air_temperature != nullptr)
          {
            component.return_air_temperature->publish_state(return_air);
          }
          component.current_temperature = return_air;
        }
        if (component.exhaust_air_temperature != nullptr && (present & PRESENT_EXHAUST_AIR) != 0)
        {
          component.exhaust_air_temperature->publish_state(decode_temperature(component.payload_byte(4)));
        }
        if (component.ewt_temperature != nullptr && (present & PRESENT_EWT) != 0)
        {
          component.ewt_temperature->publish_state(decode_temperature(component.payload_byte(6)));
        }
        if (component.reheating_temperature != nullptr && (present & PRESENT_REHEATING) != 0)
        {
          component.reheating_temperature->publish_state(decode_temperature(component.payload_byte(7)));
        }
        if (component.kitchen_hood_temperature != nullptr && (present & PRESENT_KITCHEN_HOOD) != 0)
        {
          component.kitchen_hood_temperature->publish_state(decode_temperature(component.payload_byte(8)));
        }
        return true;
      }

      return false;
    }

  }
}
