#include "comfoair_climate.h"

namespace esphome
{
  namespace comfoair_ca
  {
    namespace
    {
      constexpr uint8_t LEVEL_HIGH{static_cast<uint8_t>(FanLevel::High)};

      constexpr uint8_t LEVEL_MEDIUM{static_cast<uint8_t>(FanLevel::Medium)};

      constexpr uint8_t LEVEL_LOW{static_cast<uint8_t>(FanLevel::Low)};

      constexpr uint8_t LEVEL_OFF{static_cast<uint8_t>(FanLevel::Off)};

      constexpr uint8_t LEVEL_AUTO{static_cast<uint8_t>(FanLevel::Auto)};

      [[nodiscard]] int fan_mode_to_level(climate::ClimateFanMode mode) noexcept
      {
        switch (mode)
        {
        case climate::CLIMATE_FAN_HIGH:
          return LEVEL_HIGH;
        case climate::CLIMATE_FAN_MEDIUM:
          return LEVEL_MEDIUM;
        case climate::CLIMATE_FAN_LOW:
          return LEVEL_LOW;
        case climate::CLIMATE_FAN_OFF:
          return LEVEL_OFF;
        case climate::CLIMATE_FAN_AUTO:
          return LEVEL_AUTO;
        case climate::CLIMATE_FAN_ON:
        case climate::CLIMATE_FAN_MIDDLE:
        case climate::CLIMATE_FAN_DIFFUSE:
        case climate::CLIMATE_FAN_FOCUS:
        case climate::CLIMATE_FAN_QUIET:
        default:
          return -1;
        }
      }

    }

    climate::ClimateTraits ComfoAirComponent::traits()
    {
      climate::ClimateTraits traits{};

      traits.set_supported_modes({climate::CLIMATE_MODE_FAN_ONLY});
      traits.set_supported_presets({climate::CLIMATE_PRESET_HOME});

      traits.set_visual_min_temperature(MIN_COMFORT_TEMPERATURE);
      traits.set_visual_max_temperature(MAX_COMFORT_TEMPERATURE);
      traits.set_visual_temperature_step(1.0F);

      traits.set_supported_fan_modes({
          climate::CLIMATE_FAN_AUTO,
          climate::CLIMATE_FAN_LOW,
          climate::CLIMATE_FAN_MEDIUM,
          climate::CLIMATE_FAN_HIGH,
          climate::CLIMATE_FAN_OFF,
      });

      return traits;
    }

    void ComfoAirComponent::control(const climate::ClimateCall &call)
    {
      const auto requested_mode{call.get_fan_mode()};
      if (requested_mode.has_value())
      {
        fan_mode = *requested_mode;
        const int level{fan_mode_to_level(*requested_mode)};
        if (level >= 0)
        {
          LOG_INFO(TAG, "Setting level to: %i", level);
          const uint8_t payload[1]{static_cast<uint8_t>(level)};
          write_command(CMD_SET_LEVEL, payload, sizeof(payload));
        }
        else
        {
          LOG_INFO(TAG, "Ignoring unsupported fan mode: %i", static_cast<int>(*requested_mode));
        }
      }

      const auto requested_temperature{call.get_target_temperature()};
      if (requested_temperature.has_value())
      {
        target_temperature = *requested_temperature;
        if (target_temperature < MIN_COMFORT_TEMPERATURE ||
            target_temperature > MAX_COMFORT_TEMPERATURE)
        {
          LOG_INFO(TAG, "Ignoring invalid temperature request: %i",
                   static_cast<int>(target_temperature));
        }
        else
        {
          LOG_INFO(TAG, "Setting temperature to: %i", static_cast<int>(target_temperature));
          const uint8_t payload[1]{encode_comfort_temperature(target_temperature)};
          write_command(CMD_SET_COMFORT_TEMPERATURE, payload, sizeof(payload));
        }
      }

      publish_state();
    }

    void ComfoAirComponent::error_reset()
    {
      write_command(CMD_RESET_AND_SELF_TEST, ERROR_RESET_PAYLOAD,
                    sizeof(ERROR_RESET_PAYLOAD));
    }

    void ComfoAirComponent::filter_reset()
    {
      write_command(CMD_RESET_AND_SELF_TEST, FILTER_RESET_PAYLOAD,
                    sizeof(FILTER_RESET_PAYLOAD));
    }

    void ComfoAirComponent::set_supply_absent_level(uint8_t value)
    {
      supply_absent_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_exhaust_absent_level(uint8_t value)
    {
      exhaust_absent_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_supply_low_level(uint8_t value)
    {
      supply_low_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_exhaust_low_level(uint8_t value)
    {
      exhaust_low_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_supply_medium_level(uint8_t value)
    {
      supply_medium_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_exhaust_medium_level(uint8_t value)
    {
      exhaust_medium_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_supply_high_level(uint8_t value)
    {
      supply_high_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_exhaust_high_level(uint8_t value)
    {
      exhaust_high_ = value;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::set_supply_levels(uint8_t value_absent, uint8_t value_low,
                                              uint8_t value_medium, uint8_t value_high)
    {
      supply_absent_ = value_absent;
      supply_low_ = value_low;
      supply_medium_ = value_medium;
      supply_high_ = value_high;
      refresh_ventilation_levels();
    }

    void ComfoAirComponent::refresh_ventilation_levels()
    {
      VentilationLevels levels{};
      levels.exhaust_absent = exhaust_absent_;
      levels.exhaust_low = exhaust_low_;
      levels.exhaust_medium = exhaust_medium_;
      levels.supply_absent = supply_absent_;
      levels.supply_low = supply_low_;
      levels.supply_medium = supply_medium_;
      levels.exhaust_high = exhaust_high_;
      levels.supply_high = supply_high_;
      levels.unused = unused_byte_;

      if (!can_send_ventilation_levels_)
      {
        LOG_WARN(TAG, "Can't send ventilation levels yet");
        return;
      }

      uint8_t encoded[VENTILATION_LEVEL_PAYLOAD_SIZE]{};
      encode_ventilation_levels(levels, encoded);
      write_command(CMD_SET_VENTILATION_LEVEL, encoded, sizeof(encoded));
    }

  }
}
