
#include "parse_ventilation.h"

#include "comfoair.h"

namespace esphome
{
  namespace comfoair_ca
  {
    namespace
    {
      [[nodiscard]] bool level_to_fan_mode(uint8_t level, climate::ClimateFanMode &mode) noexcept
      {
        switch (level)
        {
        case 0x00:
          mode = climate::CLIMATE_FAN_AUTO;
          return true;
        case 0x01:
          mode = climate::CLIMATE_FAN_OFF;
          return true;
        case 0x02:
          mode = climate::CLIMATE_FAN_LOW;
          return true;
        case 0x03:
          mode = climate::CLIMATE_FAN_MEDIUM;
          return true;
        case 0x04:
          mode = climate::CLIMATE_FAN_HIGH;
          return true;
        default:
          return false;
        }
      }
    }

    bool decode_ventilation(ComfoAirComponent &component)
    {
      const uint8_t command{component.data_[COMMAND_IDX_MSG_ID]};
      const uint8_t length{component.payload_length()};

      if (command == RES_GET_SENSOR_DATA)
      {
        if (length < SENSOR_DATA_MIN_SIZE)
        {
          LOG_WARN(TAG, "Sensor data: expected %i bytes, got %i",
                   static_cast<int>(SENSOR_DATA_MIN_SIZE), static_cast<int>(length));
          return false;
        }
        if (component.enthalpy_temperature != nullptr)
        {
          component.enthalpy_temperature->publish_state(
              (static_cast<float>(component.payload_byte(0)) / TEMPERATURE_DIVISOR) - TEMPERATURE_OFFSET);
        }
        return true;
      }

      if (command == RES_GET_VENTILATION_LEVEL)
      {
        if (length < VENTILATION_LEVEL_MIN_SIZE)
        {
          LOG_WARN(TAG, "Ventilation level: expected %i bytes, got %i",
                   static_cast<int>(VENTILATION_LEVEL_MIN_SIZE), static_cast<int>(length));
          return false;
        }

        component.exhaust_absent_ = component.payload_byte(VENT_IDX_EXHAUST_ABSENT);
        component.exhaust_low_ = component.payload_byte(VENT_IDX_EXHAUST_LOW);
        component.exhaust_medium_ = component.payload_byte(VENT_IDX_EXHAUST_MEDIUM);
        component.supply_absent_ = component.payload_byte(VENT_IDX_SUPPLY_ABSENT);
        component.supply_low_ = component.payload_byte(VENT_IDX_SUPPLY_LOW);
        component.supply_medium_ = component.payload_byte(VENT_IDX_SUPPLY_MEDIUM);
        component.exhaust_high_ = component.payload_byte(VENT_IDX_EXHAUST_HIGH);
        component.supply_high_ = component.payload_byte(VENT_IDX_SUPPLY_HIGH);

        component.can_send_ventilation_levels_ = true;

        if (component.return_air_level != nullptr)
        {
          component.return_air_level->publish_state(component.payload_byte(VENT_IDX_RETURN_AIR_LEVEL));
        }
        if (component.supply_air_level != nullptr)
        {
          component.supply_air_level->publish_state(component.payload_byte(VENT_IDX_SUPPLY_AIR_LEVEL));
        }

        const uint8_t current_level{component.payload_byte(VENT_IDX_CURRENT_LEVEL)};
        if (component.ventilation_level != nullptr)
        {
          component.ventilation_level->publish_state(
              static_cast<float>(current_level) - 1.0F);
        }

        climate::ClimateFanMode decoded_mode{climate::CLIMATE_FAN_AUTO};
        if (level_to_fan_mode(current_level, decoded_mode))
        {
          component.fan_mode = decoded_mode;
          component.mode = climate::CLIMATE_MODE_FAN_ONLY;
        }
        else
        {
          LOG_VERBOSE(TAG, "Unrecognised ventilation level 0x%02X", current_level);
        }

        component.publish_state();

        if (component.supply_fan_active != nullptr)
        {
          component.supply_fan_active->publish_state(component.payload_byte(VENT_IDX_SUPPLY_FAN_ACTIVE) == 1);
        }
        return true;
      }

      return false;
    }

  }
}
