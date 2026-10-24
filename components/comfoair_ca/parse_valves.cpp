#include "parse_valves.h"

#include "comfoair.h"

namespace esphome
{
  namespace comfoair_ca
  {
    bool decode_valve_status(ComfoAirComponent &component)
    {
      const uint8_t command{component.data_[COMMAND_IDX_MSG_ID]};
      const uint8_t length{component.payload_length()};

      if (command == RES_GET_VALVE_STATUS)
      {
        if (length < VALVE_STATUS_MIN_SIZE)
        {
          LOG_WARN(TAG, "Valve status: expected %i bytes, got %i",
                   static_cast<int>(VALVE_STATUS_MIN_SIZE), static_cast<int>(length));
          return false;
        }
        if (component.bypass_valve != nullptr)
        {
          component.bypass_valve->publish_state(component.payload_byte(0));
        }
        if (component.bypass_valve_open != nullptr)
        {
          component.bypass_valve_open->publish_state(component.payload_byte(0) != 0);
        }
        if (component.preheating_state != nullptr)
        {
          component.preheating_state->publish_state(component.payload_byte(1) != 0);
        }
        if (component.motor_current_bypass != nullptr)
        {
          component.motor_current_bypass->publish_state(component.payload_byte(2));
        }
        if (component.motor_current_preheating != nullptr)
        {
          component.motor_current_preheating->publish_state(component.payload_byte(3));
        }
        return true;
      }

      if (command == RES_GET_BYPASS_CONTROL_STATUS)
      {
        if (length < BYPASS_CONTROL_MIN_SIZE)
        {
          LOG_WARN(TAG, "Bypass control status: expected %i bytes, got %i",
                   static_cast<int>(BYPASS_CONTROL_MIN_SIZE), static_cast<int>(length));
          return false;
        }
        if (component.bypass_factor != nullptr)
        {
          component.bypass_factor->publish_state(component.payload_byte(2));
        }
        if (component.bypass_step != nullptr)
        {
          component.bypass_step->publish_state(component.payload_byte(3));
        }
        if (component.bypass_correction != nullptr)
        {
          component.bypass_correction->publish_state(component.payload_byte(4));
        }
        if (component.summer_mode != nullptr)
        {
          component.summer_mode->publish_state(component.payload_byte(6) != 0);
        }
        return true;
      }

      if (command == RES_GET_PREHEATING_STATUS)
      {
        if (length < PREHEATING_MIN_SIZE)
        {
          LOG_WARN(TAG, "Preheating status: expected %i bytes, got %i",
                   static_cast<int>(PREHEATING_MIN_SIZE), static_cast<int>(length));
          return false;
        }

        if (component.preheating_valve != nullptr)
        {
          const char *valve_name{"Unknown"};
          switch (component.payload_byte(0))
          {
          case 0:
            valve_name = "Closed";
            break;
          case 1:
            valve_name = "Open";
            break;
          default:
            break;
          }
          component.preheating_valve->publish_state(valve_name);
        }

        if (component.frost_protection_active != nullptr)
        {
          component.frost_protection_active->publish_state(component.payload_byte(1) != 0);
        }
        if (component.preheating_state != nullptr)
        {
          component.preheating_state->publish_state(component.payload_byte(2) != 0);
        }
        if (component.frost_protection_minutes != nullptr)
        {
          component.frost_protection_minutes->publish_state(component.payload_word(3));
        }

        if (component.frost_protection_level != nullptr)
        {
          const char *level_name{"Unknown"};
          switch (component.payload_byte(5))
          {
          case 0:
            level_name = "GuaranteedProtection";
            break;
          case 1:
            level_name = "HighProtection";
            break;
          case 2:
            level_name = "NominalProtection";
            break;
          case 3:
            level_name = "Economy";
            break;
          default:
            break;
          }
          component.frost_protection_level->publish_state(level_name);
        }
        return true;
      }

      return false;
    }

  }
}
