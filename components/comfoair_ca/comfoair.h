#pragma once

#include <cstdint>
#include <string>

#include "esphome/core/component.h"
#include "esphome/core/optional.h"
#include "esphome/core/preferences.h"
#include "esphome/core/preference_backend.h"

#include "esphome/components/uart/uart.h"
#include "esphome/components/climate/climate.h"
#include "esphome/components/climate/climate_mode.h"
#include "esphome/components/climate/climate_traits.h"
#include "esphome/components/sensor/sensor.h"
#include "esphome/components/binary_sensor/binary_sensor.h"
#include "esphome/components/text_sensor/text_sensor.h"

#include "checksum.h"
#include "commands.h"
#include "frame.h"
#include "rpm_filter.h"
#include "logger.h"
#include "registers.h"

namespace esphome
{
  namespace comfoair_ca
  {
    inline const char *const TAG{"comfoair_ca"};

    inline constexpr uint32_t PREFERENCE_DEVELOPER_MODE{0x12345678};
    inline constexpr uint32_t POLL_INTERVAL_MS{600};

    inline constexpr std::size_t TRACE_CHARS_PER_BYTE{3};
    
    class ComfoAirComponent;

    bool decode_device_info(ComfoAirComponent &component);
    bool decode_fan_status(ComfoAirComponent &component);
    bool decode_valve_status(ComfoAirComponent &component);
    bool decode_temperatures(ComfoAirComponent &component);
    bool decode_ventilation(ComfoAirComponent &component);
    bool decode_status(ComfoAirComponent &component);
    bool decode_counters(ComfoAirComponent &component);

    class ComfoAirComponent : public climate::Climate,
                              public PollingComponent,
                              public uart::UARTDevice
    {
    public:
      ComfoAirComponent() : Climate(), PollingComponent(POLL_INTERVAL_MS), UARTDevice() {}

      void setup() override;
      void dump_config() override;
      void update() override;
      void loop() override;
      float get_setup_priority() const override { return setup_priority::DATA; }

      climate::ClimateTraits traits() override;
      void control(const climate::ClimateCall &call) override;

      void error_reset();
      void filter_reset();

      void set_name(const char *value) { name_ = value; }
      void set_name(const char *value, uint32_t name_hash)
      {
        (void)name_hash;
        name_ = value;
      }
      void set_uart_component(uart::UARTComponent *parent) { set_uart_parent(parent); }

      void set_developer_mode(LogLevel level);
      LogLevel get_developer_mode() const { return logLevel; }

      void advance_update_counter();
      int8_t update_counter() const { return update_counter_; }

      uint8_t get_supply_absent_level() const { return supply_absent_; }
      uint8_t get_exhaust_absent_level() const { return exhaust_absent_; }
      uint8_t get_supply_low_level() const { return supply_low_; }
      uint8_t get_exhaust_low_level() const { return exhaust_low_; }
      uint8_t get_supply_medium_level() const { return supply_medium_; }
      uint8_t get_exhaust_medium_level() const { return exhaust_medium_; }
      uint8_t get_supply_high_level() const { return supply_high_; }
      uint8_t get_exhaust_high_level() const { return exhaust_high_; }

      void set_supply_absent_level(uint8_t value);
      void set_exhaust_absent_level(uint8_t value);
      void set_supply_low_level(uint8_t value);
      void set_exhaust_low_level(uint8_t value);
      void set_supply_medium_level(uint8_t value);
      void set_exhaust_medium_level(uint8_t value);
      void set_supply_high_level(uint8_t value);
      void set_exhaust_high_level(uint8_t value);

      void set_supply_levels(uint8_t value_absent, uint8_t value_low,
                             uint8_t value_medium, uint8_t value_high);

      text_sensor::TextSensor *bootloader_info{nullptr};
      text_sensor::TextSensor *firmware_info{nullptr};
      text_sensor::TextSensor *connector_board_info{nullptr};
      text_sensor::TextSensor *type{nullptr};
      text_sensor::TextSensor *size{nullptr};
      text_sensor::TextSensor *filter_status{nullptr};
      text_sensor::TextSensor *frost_protection_level{nullptr};
      text_sensor::TextSensor *preheating_valve{nullptr};

      sensor::Sensor *intake_fan_speed{nullptr};
      sensor::Sensor *exhaust_fan_speed{nullptr};
      sensor::Sensor *intake_fan_speed_rpm{nullptr};
      sensor::Sensor *exhaust_fan_speed_rpm{nullptr};
      sensor::Sensor *ventilation_level{nullptr};
      sensor::Sensor *outside_air_temperature{nullptr};
      sensor::Sensor *supply_air_temperature{nullptr};
      sensor::Sensor *return_air_temperature{nullptr};
      sensor::Sensor *exhaust_air_temperature{nullptr};
      sensor::Sensor *enthalpy_temperature{nullptr};
      sensor::Sensor *ewt_temperature{nullptr};
      sensor::Sensor *reheating_temperature{nullptr};
      sensor::Sensor *kitchen_hood_temperature{nullptr};
      sensor::Sensor *return_air_level{nullptr};
      sensor::Sensor *supply_air_level{nullptr};
      sensor::Sensor *bypass_valve{nullptr};
      sensor::Sensor *bypass_factor{nullptr};
      sensor::Sensor *bypass_step{nullptr};
      sensor::Sensor *bypass_correction{nullptr};
      sensor::Sensor *bypass_open_hours{nullptr};
      sensor::Sensor *motor_current_bypass{nullptr};
      sensor::Sensor *motor_current_preheating{nullptr};
      sensor::Sensor *preheating_hours{nullptr};
      sensor::Sensor *level0_hours{nullptr};
      sensor::Sensor *level1_hours{nullptr};
      sensor::Sensor *level2_hours{nullptr};
      sensor::Sensor *level3_hours{nullptr};
      sensor::Sensor *frost_protection_hours{nullptr};
      sensor::Sensor *frost_protection_minutes{nullptr};
      sensor::Sensor *filter_hours{nullptr};
      sensor::Sensor *bathroom_switch_on_delay_minutes{nullptr};
      sensor::Sensor *bathroom_switch_off_delay_minutes{nullptr};
      sensor::Sensor *l1_switch_off_delay_minutes{nullptr};
      sensor::Sensor *boost_ventilation_minutes{nullptr};
      sensor::Sensor *filter_warning_weeks{nullptr};
      sensor::Sensor *rf_high_time_short_minutes{nullptr};
      sensor::Sensor *rf_high_time_long_minutes{nullptr};
      sensor::Sensor *extractor_hood_switch_off_delay_minutes{nullptr};

      binary_sensor::BinarySensor *bypass_present{nullptr};
      binary_sensor::BinarySensor *enthalpy_present{nullptr};
      binary_sensor::BinarySensor *ewt_present{nullptr};
      binary_sensor::BinarySensor *options_present{nullptr};
      binary_sensor::BinarySensor *fireplace_present{nullptr};
      binary_sensor::BinarySensor *kitchen_hood_present{nullptr};
      binary_sensor::BinarySensor *postheating_present{nullptr};
      binary_sensor::BinarySensor *postheating_pwm_mode_present{nullptr};
      binary_sensor::BinarySensor *preheating_present{nullptr};
      binary_sensor::BinarySensor *bypass_valve_open{nullptr};
      binary_sensor::BinarySensor *preheating_state{nullptr};
      binary_sensor::BinarySensor *summer_mode{nullptr};
      binary_sensor::BinarySensor *supply_fan_active{nullptr};
      binary_sensor::BinarySensor *frost_protection_active{nullptr};

      binary_sensor::BinarySensor *p10_active{nullptr};
      binary_sensor::BinarySensor *p11_active{nullptr};
      binary_sensor::BinarySensor *p12_active{nullptr};
      binary_sensor::BinarySensor *p13_active{nullptr};
      binary_sensor::BinarySensor *p14_active{nullptr};
      binary_sensor::BinarySensor *p15_active{nullptr};
      binary_sensor::BinarySensor *p16_active{nullptr};
      binary_sensor::BinarySensor *p17_active{nullptr};
      binary_sensor::BinarySensor *p18_active{nullptr};
      binary_sensor::BinarySensor *p19_active{nullptr};
      binary_sensor::BinarySensor *p90_active{nullptr};
      binary_sensor::BinarySensor *p91_active{nullptr};
      binary_sensor::BinarySensor *p92_active{nullptr};
      binary_sensor::BinarySensor *p93_active{nullptr};
      binary_sensor::BinarySensor *p94_active{nullptr};
      binary_sensor::BinarySensor *p95_active{nullptr};
      binary_sensor::BinarySensor *p96_active{nullptr};
      binary_sensor::BinarySensor *p97_active{nullptr};

      void set_bootloader_info(text_sensor::TextSensor *v) { bootloader_info = v; }
      void set_firmware_info(text_sensor::TextSensor *v) { firmware_info = v; }
      void set_connector_board_info(text_sensor::TextSensor *v) { connector_board_info = v; }
      void set_type(text_sensor::TextSensor *v) { type = v; }
      void set_size(text_sensor::TextSensor *v) { size = v; }
      void set_filter_status(text_sensor::TextSensor *v) { filter_status = v; }
      void set_frost_protection_level(text_sensor::TextSensor *v) { frost_protection_level = v; }
      void set_preheating_valve(text_sensor::TextSensor *v) { preheating_valve = v; }

      void set_intake_fan_speed(sensor::Sensor *v) { intake_fan_speed = v; }
      void set_exhaust_fan_speed(sensor::Sensor *v) { exhaust_fan_speed = v; }
      void set_intake_fan_speed_rpm(sensor::Sensor *v)
      {
        intake_fan_speed_rpm = v;
        intake_fan_speed_rpm->set_accuracy_decimals(0);
      }
      void set_exhaust_fan_speed_rpm(sensor::Sensor *v)
      {
        exhaust_fan_speed_rpm = v;
        exhaust_fan_speed_rpm->set_accuracy_decimals(0);
      }
      void set_ventilation_level(sensor::Sensor *v) { ventilation_level = v; }
      void set_outside_air_temperature(sensor::Sensor *v) { outside_air_temperature = v; }
      void set_supply_air_temperature(sensor::Sensor *v) { supply_air_temperature = v; }
      void set_return_air_temperature(sensor::Sensor *v) { return_air_temperature = v; }
      void set_exhaust_air_temperature(sensor::Sensor *v) { exhaust_air_temperature = v; }
      void set_enthalpy_temperature(sensor::Sensor *v) { enthalpy_temperature = v; }
      void set_ewt_temperature(sensor::Sensor *v) { ewt_temperature = v; }
      void set_reheating_temperature(sensor::Sensor *v) { reheating_temperature = v; }
      void set_kitchen_hood_temperature(sensor::Sensor *v) { kitchen_hood_temperature = v; }
      void set_return_air_level(sensor::Sensor *v) { return_air_level = v; }
      void set_supply_air_level(sensor::Sensor *v) { supply_air_level = v; }
      void set_bypass_valve(sensor::Sensor *v) { bypass_valve = v; }
      void set_bypass_factor(sensor::Sensor *v) { bypass_factor = v; }
      void set_bypass_step(sensor::Sensor *v) { bypass_step = v; }
      void set_bypass_correction(sensor::Sensor *v) { bypass_correction = v; }
      void set_bypass_open_hours(sensor::Sensor *v) { bypass_open_hours = v; }
      void set_motor_current_bypass(sensor::Sensor *v) { motor_current_bypass = v; }
      void set_motor_current_preheating(sensor::Sensor *v) { motor_current_preheating = v; }
      void set_preheating_hours(sensor::Sensor *v) { preheating_hours = v; }
      void set_level0_hours(sensor::Sensor *v) { level0_hours = v; }
      void set_level1_hours(sensor::Sensor *v) { level1_hours = v; }
      void set_level2_hours(sensor::Sensor *v) { level2_hours = v; }
      void set_level3_hours(sensor::Sensor *v) { level3_hours = v; }
      void set_frost_protection_hours(sensor::Sensor *v) { frost_protection_hours = v; }
      void set_frost_protection_minutes(sensor::Sensor *v) { frost_protection_minutes = v; }
      void set_filter_hours(sensor::Sensor *v) { filter_hours = v; }
      void set_bathroom_switch_on_delay_minutes(sensor::Sensor *v) { bathroom_switch_on_delay_minutes = v; }
      void set_bathroom_switch_off_delay_minutes(sensor::Sensor *v) { bathroom_switch_off_delay_minutes = v; }
      void set_l1_switch_off_delay_minutes(sensor::Sensor *v) { l1_switch_off_delay_minutes = v; }
      void set_boost_ventilation_minutes(sensor::Sensor *v) { boost_ventilation_minutes = v; }
      void set_filter_warning_weeks(sensor::Sensor *v) { filter_warning_weeks = v; }
      void set_rf_high_time_short_minutes(sensor::Sensor *v) { rf_high_time_short_minutes = v; }
      void set_rf_high_time_long_minutes(sensor::Sensor *v) { rf_high_time_long_minutes = v; }
      void set_extractor_hood_switch_off_delay_minutes(sensor::Sensor *v) { extractor_hood_switch_off_delay_minutes = v; }

      void set_bypass_present(binary_sensor::BinarySensor *v) { bypass_present = v; }
      void set_enthalpy_present(binary_sensor::BinarySensor *v) { enthalpy_present = v; }
      void set_ewt_present(binary_sensor::BinarySensor *v) { ewt_present = v; }
      void set_options_present(binary_sensor::BinarySensor *v) { options_present = v; }
      void set_fireplace_present(binary_sensor::BinarySensor *v) { fireplace_present = v; }
      void set_kitchen_hood_present(binary_sensor::BinarySensor *v) { kitchen_hood_present = v; }
      void set_postheating_present(binary_sensor::BinarySensor *v) { postheating_present = v; }
      void set_postheating_pwm_mode_present(binary_sensor::BinarySensor *v) { postheating_pwm_mode_present = v; }
      void set_preheating_present(binary_sensor::BinarySensor *v) { preheating_present = v; }
      void set_bypass_valve_open(binary_sensor::BinarySensor *v) { bypass_valve_open = v; }
      void set_preheating_state(binary_sensor::BinarySensor *v) { preheating_state = v; }
      void set_summer_mode(binary_sensor::BinarySensor *v) { summer_mode = v; }
      void set_supply_fan_active(binary_sensor::BinarySensor *v) { supply_fan_active = v; }
      void set_frost_protection_active(binary_sensor::BinarySensor *v) { frost_protection_active = v; }

      void set_p10_active(binary_sensor::BinarySensor *v) { p10_active = v; }
      void set_p11_active(binary_sensor::BinarySensor *v) { p11_active = v; }
      void set_p12_active(binary_sensor::BinarySensor *v) { p12_active = v; }
      void set_p13_active(binary_sensor::BinarySensor *v) { p13_active = v; }
      void set_p14_active(binary_sensor::BinarySensor *v) { p14_active = v; }
      void set_p15_active(binary_sensor::BinarySensor *v) { p15_active = v; }
      void set_p16_active(binary_sensor::BinarySensor *v) { p16_active = v; }
      void set_p17_active(binary_sensor::BinarySensor *v) { p17_active = v; }
      void set_p18_active(binary_sensor::BinarySensor *v) { p18_active = v; }
      void set_p19_active(binary_sensor::BinarySensor *v) { p19_active = v; }
      void set_p90_active(binary_sensor::BinarySensor *v) { p90_active = v; }
      void set_p91_active(binary_sensor::BinarySensor *v) { p91_active = v; }
      void set_p92_active(binary_sensor::BinarySensor *v) { p92_active = v; }
      void set_p93_active(binary_sensor::BinarySensor *v) { p93_active = v; }
      void set_p94_active(binary_sensor::BinarySensor *v) { p94_active = v; }
      void set_p95_active(binary_sensor::BinarySensor *v) { p95_active = v; }
      void set_p96_active(binary_sensor::BinarySensor *v) { p96_active = v; }
      void set_p97_active(binary_sensor::BinarySensor *v) { p97_active = v; }

    protected:
      friend bool decode_device_info(ComfoAirComponent &component);
      friend bool decode_fan_status(ComfoAirComponent &component);
      friend bool decode_valve_status(ComfoAirComponent &component);
      friend bool decode_temperatures(ComfoAirComponent &component);
      friend bool decode_ventilation(ComfoAirComponent &component);
      friend bool decode_status(ComfoAirComponent &component);
      friend bool decode_counters(ComfoAirComponent &component);

      void write_command(uint8_t command, const uint8_t *payload, uint8_t payload_length);
      void write_tx(const uint8_t *data, std::size_t length);
      bool command_payload_fits(uint8_t payload_length) const;

      void feed_byte(uint8_t byte);
      void parse_data();
      void reset_frame();
      void maybe_retry();

      void raw_trace_dump(const char *level_tag) const;

      uint8_t payload_byte(uint8_t index) const;
      uint16_t payload_word(uint8_t index) const;
      uint8_t payload_length() const;

      std::string filter_ascii(const char *input) const;
      void refresh_ventilation_levels();

      uint8_t data_[COMMAND_MAX_LENGTH]{};
      std::size_t data_index_{0};
      bool skip_escaped_07_{false};
      uint32_t frame_start_ms_{0};

      bool retry_pending_{false};
      uint32_t last_retry_ms_{0};
      uint8_t last_command_{0};
      uint8_t last_payload_length_{0};
      uint8_t last_payload_[COMMAND_MAX_LENGTH]{};

      int8_t update_counter_{-4};
      static constexpr int8_t LAST_UPDATE_STEP{9};

      uint8_t bootloader_version_[13]{};
      uint8_t firmware_version_[13]{};
      uint8_t connector_board_version_[14]{};
      const char *name_{nullptr};

      uint8_t supply_absent_{0};
      uint8_t exhaust_absent_{0};
      uint8_t supply_low_{0};
      uint8_t exhaust_low_{0};
      uint8_t supply_medium_{0};
      uint8_t exhaust_medium_{0};
      uint8_t supply_high_{0};
      uint8_t exhaust_high_{0};
      uint8_t unused_byte_{0};
      bool can_send_ventilation_levels_{false};

      MedianFilter3 intake_rpm_filter{};
      MedianFilter3 exhaust_rpm_filter{};

      ESPPreferenceObject pref_;
    };

  } // namespace comfoair_ca
} // namespace esphome
