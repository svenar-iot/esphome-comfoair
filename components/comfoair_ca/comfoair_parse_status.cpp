
#include "comfoair_parse_status.h"

#include "comfoair.h"

#include <array>

namespace esphome
{
  namespace comfoair_ca
  {
    namespace
    {
      struct FaultBit
      {
        uint8_t byte_index;
        uint8_t mask;
        binary_sensor::BinarySensor *ComfoAirComponent::*sensor;
      };

      constexpr std::array<FaultBit, 18> FAULT_BITS{{
          {STATUS_IDX_P10_Q1, 0x01, &ComfoAirComponent::p10_active},
          {STATUS_IDX_P10_Q1, 0x02, &ComfoAirComponent::p11_active},
          {STATUS_IDX_P10_Q1, 0x04, &ComfoAirComponent::p12_active},
          {STATUS_IDX_P10_Q1, 0x08, &ComfoAirComponent::p13_active},
          {STATUS_IDX_P10_Q1, 0x10, &ComfoAirComponent::p14_active},
          {STATUS_IDX_P10_Q1, 0x20, &ComfoAirComponent::p15_active},
          {STATUS_IDX_P10_Q1, 0x40, &ComfoAirComponent::p16_active},
          {STATUS_IDX_P10_Q1, 0x80, &ComfoAirComponent::p17_active},
          {STATUS_IDX_P18_Q1, 0x01, &ComfoAirComponent::p18_active},
          {STATUS_IDX_P18_Q1, 0x02, &ComfoAirComponent::p19_active},
          {STATUS_IDX_P90_Q1, 0x01, &ComfoAirComponent::p90_active},
          {STATUS_IDX_P90_Q1, 0x02, &ComfoAirComponent::p91_active},
          {STATUS_IDX_P90_Q1, 0x04, &ComfoAirComponent::p92_active},
          {STATUS_IDX_P90_Q1, 0x08, &ComfoAirComponent::p93_active},
          {STATUS_IDX_P90_Q1, 0x10, &ComfoAirComponent::p94_active},
          {STATUS_IDX_P90_Q1, 0x20, &ComfoAirComponent::p95_active},
          {STATUS_IDX_P90_Q1, 0x40, &ComfoAirComponent::p96_active},
          {STATUS_IDX_P90_Q1, 0x80, &ComfoAirComponent::p97_active},
      }};

      [[nodiscard]] const char *type_name(uint8_t value) noexcept
      {
        switch (value)
        {
        case 1:
          return "Left";
        case 2:
          return "Right";
        default:
          return "Unknown";
        }
      }

      [[nodiscard]] const char *size_name(uint8_t value) noexcept
      {
        switch (value)
        {
        case 1:
          return "Large";
        case 2:
          return "Small";
        default:
          return "Unknown";
        }
      }
    }

    bool decode_status(ComfoAirComponent &component)
    {
      const uint8_t command{component.data_[COMMAND_IDX_MSG_ID]};
      const uint8_t length{component.payload_length()};

      if (command == RES_GET_FAULTS)
      {
        if (length < FAULTS_MIN_SIZE)
        {
          LOG_WARN(TAG, "Faults: expected %i bytes, got %i",
                   static_cast<int>(FAULTS_MIN_SIZE), static_cast<int>(length));
          return false;
        }
        if (component.filter_status != nullptr)
        {
          const uint8_t status{component.payload_byte(8)};
          component.filter_status->publish_state(status == 0 ? "Ok" : (status == 1 ? "Full" : "Unknown"));
        }
        return true;
      }

      if (command == RES_GET_STATUS)
      {
        if (length < STATUS_MIN_SIZE)
        {
          LOG_WARN(TAG, "Status: expected %i bytes, got %i",
                   static_cast<int>(STATUS_MIN_SIZE), static_cast<int>(length));
          return false;
        }

        if (component.preheating_present != nullptr)
        {
          component.preheating_present->publish_state(component.payload_byte(STATUS_IDX_PREHEATING) != 0);
        }
        if (component.bypass_present != nullptr)
        {
          component.bypass_present->publish_state(component.payload_byte(STATUS_IDX_BYPASS) != 0);
        }
        if (component.type != nullptr)
        {
          component.type->publish_state(type_name(component.payload_byte(STATUS_IDX_TYPE)));
        }
        if (component.size != nullptr)
        {
          component.size->publish_state(size_name(component.payload_byte(STATUS_IDX_SIZE)));
        }

        const uint8_t options{component.payload_byte(STATUS_IDX_OPTIONS)};
        if (component.options_present != nullptr)
        {
          component.options_present->publish_state(options != 0);
        }
        if (component.fireplace_present != nullptr)
        {
          component.fireplace_present->publish_state((options & STATUS_OPTION_FIREPLACE) != 0);
        }
        if (component.kitchen_hood_present != nullptr)
        {
          component.kitchen_hood_present->publish_state((options & STATUS_OPTION_KITCHEN_HOOD) != 0);
        }
        if (component.postheating_present != nullptr)
        {
          component.postheating_present->publish_state((options & STATUS_OPTION_POSTHEATING) != 0);
        }
        if (component.postheating_pwm_mode_present != nullptr)
        {
          component.postheating_pwm_mode_present->publish_state((options & STATUS_OPTION_POSTHEATING_PWM) != 0);
        }

        for (const FaultBit &bit : FAULT_BITS)
        {
          binary_sensor::BinarySensor *sensor{component.*(bit.sensor)};
          if (sensor != nullptr)
          {
            sensor->publish_state((component.payload_byte(bit.byte_index) & bit.mask) != 0);
          }
        }

        if (component.enthalpy_present != nullptr)
        {
          component.enthalpy_present->publish_state(component.payload_byte(STATUS_IDX_ENTHALPY) != 0);
        }
        if (component.ewt_present != nullptr)
        {
          component.ewt_present->publish_state(component.payload_byte(STATUS_IDX_EWT) != 0);
        }
        return true;
      }

      return false;
    }

  }
}
