#include "comfoair_parse_device.h"

#include "comfoair.h"

#include <cstdio>
#include <cstring>

namespace esphome
{
  namespace comfoair_ca
  {
    bool decode_device_info(ComfoAirComponent &component)
    {
      const uint8_t command{component.data_[COMMAND_IDX_MSG_ID]};
      const uint8_t length{component.payload_length()};

      if (command == RES_GET_BOOTLOADER_VERSION || command == RES_GET_FIRMWARE_VERSION)
      {
        if (length < VERSION_PAYLOAD_SIZE)
        {
          LOG_WARN(TAG, "Version response 0x%02X: expected %i bytes, got %i",
                   command, static_cast<int>(VERSION_PAYLOAD_SIZE), static_cast<int>(length));
          return false;
        }

        uint8_t *destination{
            (command == RES_GET_BOOTLOADER_VERSION) ? component.bootloader_version_ : component.firmware_version_};
        text_sensor::TextSensor *sensor{
            (command == RES_GET_BOOTLOADER_VERSION) ? component.bootloader_info : component.firmware_info};

        memcpy(destination, component.data_ + FRAME_HEADER_LENGTH, VERSION_PAYLOAD_SIZE);

        char description[DESCRIPTION_BUFFER_SIZE]{};
        snprintf(description, sizeof(description), "%.10s v%0d.%02d b%2d",
                 destination + 3, destination[0], destination[1], destination[2]);

        if (sensor != nullptr)
        {
          sensor->publish_state(component.filter_ascii(description));
        }
        return true;
      }

      if (command == RES_GET_CONNECTOR_BOARD_VERSION)
      {
        constexpr uint8_t CONNECTOR_PAYLOAD_SIZE{14};
        if (length < CONNECTOR_PAYLOAD_SIZE)
        {
          LOG_WARN(TAG, "Connector board response: expected %i bytes, got %i",
                   static_cast<int>(CONNECTOR_PAYLOAD_SIZE), static_cast<int>(length));
          return false;
        }

        memcpy(component.connector_board_version_, component.data_ + FRAME_HEADER_LENGTH, CONNECTOR_PAYLOAD_SIZE);

        char description[DESCRIPTION_BUFFER_SIZE]{};
        snprintf(description, sizeof(description), "%.10s v%0d.%02d",
                 component.connector_board_version_ + 2, component.connector_board_version_[0],
                 component.connector_board_version_[1]);

        if (component.connector_board_info != nullptr)
        {
          component.connector_board_info->publish_state(component.filter_ascii(description));
        }
        return true;
      }

      return false;
    }

  }
}
