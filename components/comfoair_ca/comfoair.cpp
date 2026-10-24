#include "comfoair.h"

#include <cstdio>
#include <cstring>

namespace esphome
{
  namespace comfoair_ca
  {
    namespace
    {
      constexpr std::size_t TX_BUFFER_CAPACITY{
          max_escaped_frame_length(COMMAND_MAX_LENGTH - FRAME_HEADER_LENGTH - FRAME_TRAILER_LENGTH)};

      constexpr uint8_t MAX_PAYLOAD_BYTES{
          static_cast<uint8_t>((COMMAND_MAX_LENGTH - 7) / 2)};

      constexpr std::size_t TRACE_CHARS_PER_BYTE{3};

      constexpr int UPDATE_STEP_FIRST{-4};

    }

    void ComfoAirComponent::setup()
    {
      pref_ = global_preferences->make_preference<bool>(PREFERENCE_DEVELOPER_MODE);
      bool stored_developer_mode{false};
      if (pref_.load(&stored_developer_mode))
      {
        logEnable = stored_developer_mode;
      }
    }

    void ComfoAirComponent::dump_config()
    {
      LOG_CONFIG(TAG, "ComfoAir:");
      LOG_CONFIG(TAG, "  Bootloader %.10s v%0d.%02d b%2d",
                 bootloader_version_ + 3, bootloader_version_[0],
                 bootloader_version_[1], bootloader_version_[2]);
      LOG_CONFIG(TAG, "  Firmware %.10s v%0d.%02d b%2d",
                 firmware_version_ + 3, firmware_version_[0],
                 firmware_version_[1], firmware_version_[2]);
      LOG_CONFIG(TAG, "  Connector Board %.10s v%0d.%02d",
                 connector_board_version_ + 2, connector_board_version_[0],
                 connector_board_version_[1]);
    }

    void ComfoAirComponent::set_developer_mode(bool enable)
    {
      logEnable = enable;
      pref_.save(&enable);
    }

    void ComfoAirComponent::advance_update_counter()
    {
      ++update_counter_;
      if (update_counter_ > LAST_UPDATE_STEP)
      {
        update_counter_ = 0;
      }
    }

    void ComfoAirComponent::update()
    {
      switch (update_counter_)
      {
      case -4:
        write_command(static_cast<uint8_t>(SimpleQuery::BootloaderVersion), nullptr, 0);
        break;
      case -3:
        write_command(static_cast<uint8_t>(SimpleQuery::FirmwareVersion), nullptr, 0);
        break;
      case -2:
        write_command(static_cast<uint8_t>(SimpleQuery::ConnectorBoardVersion), nullptr, 0);
        break;
      case -1:
        write_command(static_cast<uint8_t>(SimpleQuery::Status), nullptr, 0);
        break;
      case 0:
        if (intake_fan_speed != nullptr || exhaust_fan_speed != nullptr ||
            intake_fan_speed_rpm != nullptr || exhaust_fan_speed_rpm != nullptr)
        {
          LOG_DEBUG(TAG, "getting fan status");
          write_command(static_cast<uint8_t>(SimpleQuery::FanStatus), nullptr, 0);
        }
        break;
      case 1:
        if (bypass_valve != nullptr || bypass_valve_open != nullptr || preheating_state != nullptr)
        {
          LOG_DEBUG(TAG, "getting valve status");
          write_command(static_cast<uint8_t>(SimpleQuery::ValveStatus), nullptr, 0);
        }
        break;
      case 2:
        if (outside_air_temperature != nullptr || supply_air_temperature != nullptr ||
            return_air_temperature != nullptr || exhaust_air_temperature != nullptr ||
            enthalpy_temperature != nullptr)
        {
          LOG_DEBUG(TAG, "getting sensor data");
          write_command(static_cast<uint8_t>(SimpleQuery::SensorData), nullptr, 0);
        }
        break;
      case 3:
        LOG_DEBUG(TAG, "getting ventilation level");
        write_command(static_cast<uint8_t>(SimpleQuery::VentilationLevel), nullptr, 0);
        break;
      case 4:
        LOG_DEBUG(TAG, "getting temperatures");
        write_command(static_cast<uint8_t>(SimpleQuery::Temperatures), nullptr, 0);
        break;
      case 5:
        if (filter_status != nullptr)
        {
          LOG_DEBUG(TAG, "getting error status");
          write_command(static_cast<uint8_t>(SimpleQuery::Faults), nullptr, 0);
        }
        break;
      case 6:
        LOG_DEBUG(TAG, "getting bypass control status");
        write_command(static_cast<uint8_t>(SimpleQuery::BypassControlStatus), nullptr, 0);
        break;
      case 7:
        LOG_DEBUG(TAG, "getting operation hours");
        write_command(static_cast<uint8_t>(SimpleQuery::OperationHours), nullptr, 0);
        break;
      case 8:
        LOG_DEBUG(TAG, "getting preheating status");
        write_command(static_cast<uint8_t>(SimpleQuery::PreheatingStatus), nullptr, 0);
        break;
      case 9:
        LOG_DEBUG(TAG, "getting time delay");
        write_command(static_cast<uint8_t>(SimpleQuery::TimeDelay), nullptr, 0);
        break;
      default:
        break;
      }

      advance_update_counter();
    }

    void ComfoAirComponent::raw_trace_append(uint8_t byte)
    {
      if (raw_trace_length_ < RAW_TRACE_CAPACITY)
      {
        raw_trace_[raw_trace_length_] = byte;
        ++raw_trace_length_;
      }
    }

    void ComfoAirComponent::raw_trace_dump(const char *level_tag) const
    {
      char line[(RAW_TRACE_CAPACITY * TRACE_CHARS_PER_BYTE) + 4 + 1];
      line[0] = 'R';
      line[1] = 'X';
      line[2] = ':';
      line[3] = ' ';
      line[4 + (raw_trace_length_ * TRACE_CHARS_PER_BYTE)] = '\0';
      for (std::size_t i{0}; i < raw_trace_length_; ++i)
      {
        snprintf(line + 4 + (i * TRACE_CHARS_PER_BYTE),
                 TRACE_CHARS_PER_BYTE + 1, "%02X ", raw_trace_[i]);
      }
      LOG_DEBUG(TAG, "%s%s(%i bytes)", level_tag, line, static_cast<int>(raw_trace_length_));
    }

    void ComfoAirComponent::reset_frame()
    {
      data_index_ = 0;
      skip_escaped_07_ = false;
      raw_trace_reset();
    }

    void ComfoAirComponent::feed_byte(uint8_t byte)
    {
      if (skip_escaped_07_)
      {
        skip_escaped_07_ = false;
        if (byte == ESCAPE_BYTE)
        {
          raw_trace_append(byte);
          return;
        }
      }

      if (data_index_ == 0)
      {
        frame_start_ms_ = millis();
      }

      raw_trace_append(byte);

      if (data_index_ >= COMMAND_MAX_LENGTH)
      {
        LOG_WARN(TAG, "Frame overflow, discarding %i bytes", static_cast<int>(data_index_));
        reset_frame();
        return;
      }

      data_[data_index_] = byte;

      const ByteCheck check{check_frame_byte(data_, data_index_)};

      switch (check.verdict)
      {
      case ByteVerdict::Complete:
        if (data_[COMMAND_ID_ACK] != FRAME_ACK)
        {
          parse_data();
        }
        else
        {
          LOG_DEBUG(TAG, "ACK");
        }
        reset_frame();
        return;

      case ByteVerdict::BadChecksum:
        raw_trace_dump("");
        LOG_WARN(TAG, "Checksum mismatch on 0x%02X: got 0x%02X expected 0x%02X",
                 data_[COMMAND_IDX_MSG_ID], byte, check.expected_checksum);
        LOG_WARN(TAG, "Frame discarded (declared %i data bytes)",
                 static_cast<int>(data_[COMMAND_IDX_DATA]));

        if (millis() - last_retry_ms_ > COMMAND_RETRY_GAP_MS)
        {
          retry_pending_ = true;
          last_retry_ms_ = millis();
        }
        reset_frame();
        return;

      case ByteVerdict::Invalid:
        LOG_VERBOSE(TAG, "Byte %i of received data frame is invalid.",
                    static_cast<int>(data_index_));
        reset_frame();
        return;

      case ByteVerdict::Valid:
      default:
        break;
      }

      if (byte == ESCAPE_BYTE &&
          index_is_escaped_region(data_index_, data_[COMMAND_IDX_DATA]))
      {
        skip_escaped_07_ = true;
      }

      ++data_index_;
    }

    void ComfoAirComponent::maybe_retry()
    {
      if (!retry_pending_)
      {
        return;
      }
      retry_pending_ = false;
      LOG_DEBUG(TAG, "Retrying command 0x%02X after checksum error", last_command_);
      write_command(last_command_, last_payload_length_ > 0 ? last_payload_ : nullptr,
                    last_payload_length_);
    }

    void ComfoAirComponent::loop()
    {
      if (data_index_ > 0 && millis() - frame_start_ms_ > COMMAND_FRAME_TIMEOUT_MS)
      {
        const std::size_t declared{declared_frame_length(data_)};
        LOG_WARN(TAG, "Frame incomplete after %ims (%i of %i bytes), discarding",
                 static_cast<int>(COMMAND_FRAME_TIMEOUT_MS),
                 static_cast<int>(data_index_),
                 static_cast<int>(declared > 0 ? declared : FRAME_HEADER_LENGTH));
        reset_frame();
      }

      if (available() == 0)
      {
        maybe_retry();
      }

      while (available() != 0)
      {
        uint8_t byte{0};
        if (!read_byte(&byte))
        {
          break;
        }
        feed_byte(byte);
      }
    }

    bool ComfoAirComponent::command_payload_fits(uint8_t payload_length) const
    {
      return payload_length <= MAX_PAYLOAD_BYTES;
    }

    void ComfoAirComponent::write_command(uint8_t command, const uint8_t *payload,
                                          uint8_t payload_length)
    {
      last_command_ = command;
      last_payload_length_ = 0;
      if (payload != nullptr && payload_length > 0 && payload_length <= COMMAND_MAX_LENGTH)
      {
        last_payload_length_ = payload_length;
        memcpy(last_payload_, payload, payload_length);
      }

      if (!command_payload_fits(payload_length))
      {
        LOG_WARN(TAG, "Command 0x%02X payload too large (%i bytes), not sent",
                 command, static_cast<int>(payload_length));
        return;
      }

      uint8_t message[TX_BUFFER_CAPACITY]{};
      uint8_t index{0};

      message[index++] = FRAME_PREFIX;
      message[index++] = FRAME_HEAD;

      message[index++] = 0x00;
      index = append_escaped(message, index, command);
      index = append_escaped(message, index, payload_length);

      for (uint8_t i{0}; i < payload_length; ++i)
      {
        index = append_escaped(message, index, payload[i]);
      }

      const uint8_t checksum{checksum_bytes(payload, payload_length)};
      const uint8_t total{static_cast<uint8_t>(checksum + command + payload_length)};
      index = append_escaped(message, index, total);

      // Delimiters are never escaped.
      message[index++] = FRAME_PREFIX;
      message[index++] = FRAME_TAIL;

      write_tx(message, index);
    }

    void ComfoAirComponent::write_tx(const uint8_t *data, std::size_t length)
    {
      char line[(TX_BUFFER_CAPACITY * TRACE_CHARS_PER_BYTE) + 4 + 1];
      line[0] = 'T';
      line[1] = 'X';
      line[2] = ':';
      line[3] = ' ';
      line[4 + (length * TRACE_CHARS_PER_BYTE)] = '\0';
      for (std::size_t i{0}; i < length; ++i)
      {
        snprintf(line + 4 + (i * TRACE_CHARS_PER_BYTE),
                 TRACE_CHARS_PER_BYTE + 1, "%02X ", data[i]);
      }
      LOG_DEBUG(TAG, "%s", line);

      write_array(data, length);
      flush();
    }

    uint8_t ComfoAirComponent::payload_length() const
    {
      return data_[COMMAND_IDX_DATA];
    }

    uint8_t ComfoAirComponent::payload_byte(uint8_t index) const
    {
      return data_[FRAME_HEADER_LENGTH + index];
    }

    uint16_t ComfoAirComponent::payload_word(uint8_t index) const
    {
      // Big-endian per the protocol documentation.
      const uint16_t high{payload_byte(index)};
      const uint16_t low{payload_byte(static_cast<uint8_t>(index + 1))};
      return static_cast<uint16_t>((high << 8) | low);
    }

    std::string ComfoAirComponent::filter_ascii(const char *input) const
    {
      std::string out;
      if (input == nullptr)
      {
        return out;
      }
      for (std::size_t i{0}; input[i] != '\0'; ++i)
      {
        const char c{input[i]};
        if (c >= 32 && c <= 126)
        {
          out += c;
        }
      }
      return out;
    }

    void ComfoAirComponent::parse_data()
    {
      raw_trace_dump("");
      status_clear_warning();

      const uint8_t command{data_[COMMAND_IDX_MSG_ID]};
      bool handled{false};

      switch (command)
      {
      case RES_GET_BOOTLOADER_VERSION:
      case RES_GET_FIRMWARE_VERSION:
      case RES_GET_CONNECTOR_BOARD_VERSION:
        handled = decode_device_info(*this);
        break;

      case RES_GET_FAN_STATUS:
        handled = decode_fan_status(*this);
        break;

      case RES_GET_VALVE_STATUS:
      case RES_GET_BYPASS_CONTROL_STATUS:
      case RES_GET_PREHEATING_STATUS:
        handled = decode_valve_status(*this);
        break;

      case RES_GET_TEMPERATURE_STATUS:
      case RES_GET_TEMPERATURES:
        handled = decode_temperatures(*this);
        break;

      case RES_GET_SENSOR_DATA:
      case RES_GET_VENTILATION_LEVEL:
        handled = decode_ventilation(*this);
        break;

      case RES_GET_FAULTS:
      case RES_GET_STATUS:
        handled = decode_status(*this);
        break;

      case RES_GET_OPERATION_HOURS:
      case RES_GET_TIME_DELAY:
        handled = decode_counters(*this);
        break;

      default:
        LOG_VERBOSE(TAG, "Unhandled response 0x%02X", command);
        break;
      }

      if (!handled)
      {
        LOG_DEBUG(TAG, "Response 0x%02X not decoded (payload %i bytes)",
                  command, static_cast<int>(payload_length()));
      }
    }

  }
}
