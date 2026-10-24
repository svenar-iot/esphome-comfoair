#include "comfoair_parse_fans.h"

#include "comfoair.h"

namespace esphome
{
  namespace comfoair_ca
  {
    bool decode_fan_status(ComfoAirComponent &component)
    {
      const uint8_t length{component.payload_length()};
      if (length < FAN_STATUS_PAYLOAD_SIZE)
      {
        LOG_WARN(TAG, "Fan status: expected %i bytes, got %i",
                 static_cast<int>(FAN_STATUS_PAYLOAD_SIZE), static_cast<int>(length));
        return false;
      }

      if (component.intake_fan_speed != nullptr)
      {
        component.intake_fan_speed->publish_state(component.payload_byte(0));
      }
      if (component.exhaust_fan_speed != nullptr)
      {
        component.exhaust_fan_speed->publish_state(component.payload_byte(1));
      }

      // A zero tacho period means no reading, not infinite RPM.
      const uint16_t intake_divisor{component.payload_word(2)};
      if (component.intake_fan_speed_rpm != nullptr && intake_divisor != 0)
      {
        component.intake_fan_speed_rpm->publish_state(
            FAN_RPM_SCALE / static_cast<float>(intake_divisor));
      }

      const uint16_t exhaust_divisor{component.payload_word(4)};
      if (component.exhaust_fan_speed_rpm != nullptr && exhaust_divisor != 0)
      {
        component.exhaust_fan_speed_rpm->publish_state(
            FAN_RPM_SCALE / static_cast<float>(exhaust_divisor));
      }

      return true;
    }

  }
}
