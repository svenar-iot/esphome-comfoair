#include "comfoair_q.h"

namespace comfoair_q
{
    void ComfoAirQ::setup()
    {
        // Perform an initial request for all PDOs 10 seconds after startup.
        this->set_timeout(10000, [this]()
                          { this->update(); });
    }

    void ComfoAirQ::update()
    {
        if (!request_ids_.empty())
        {
            request_all_pdos();
        }
    }

    void ComfoAirQ::request_all_pdos()
    {
        if (request_next_pdo_pos_ != 0)
        {
            ESP_LOGW(TAG, "Skipping PDO request cycle: previous cycle still in progress.");
            return;
        }

        ESP_LOGD(TAG, "Requesting all registered PDOs (count: %d)", request_ids_.size());
        request_next_pdo_();
    }

    void ComfoAirQ::request_next_pdo_()
    {
        if (request_next_pdo_pos_ >= request_ids_.size())
        {
            request_next_pdo_pos_ = 0;
            return;
        }

        request_pdo(request_ids_[request_next_pdo_pos_++]);

        if (request_next_pdo_pos_ < request_ids_.size())
        {
            this->set_timeout(request_delay_, [this]()
                              { request_next_pdo_(); });
        }
        else
        {
            request_next_pdo_pos_ = 0;
        }
    }

    void ComfoAirQ::request_pdo(uint16_t pdo_id)
    {
        // ComfoAirQ does not care for the data length to be set corectly on transmission requests, so there is no need to send any data
        this->send_can_message_((pdo_id << 14) + 0x40 + this->local_node_id_, true);
    }

    void ComfoAirQ::set_level(uint8_t level)
    {
        send_command_set_timer(true, 0x01, 0x01, level);
    }

    void ComfoAirQ::set_level_float(float state)
    {
        uint8_t level;
        if (state >= 1)
        {
            level = 3;
        }
        else if (state >= 0.5)
        {
            level = 2;
        }
        else if (state > 0)
        {
            level = 1;
        }
        else
        {
            level = 0;
        }
        ESP_LOGD(TAG, "send_cmd_level_float: state: %f, level: %d", state, level);

        set_level(level);
    }

    void ComfoAirQ::send_command_set_timer(bool enable, uint8_t subunit_id, uint8_t property_id, uint8_t property_value, uint32_t duration_secs)
    {
        std::vector<uint8_t> command = {(uint8_t)(enable ? 0x84 : 0x85), 0x15 /* SCHEDULE */, subunit_id, property_id};
        if (enable)
            command.insert(command.end(), {0x00, 0x00, 0x00, 0x00, (uint8_t)(duration_secs), (uint8_t)(duration_secs >> 8),
                                           (uint8_t)(duration_secs >> 16), (uint8_t)(duration_secs >> 24), property_value});
        send_command(command);
    }

    void ComfoAirQ::send_command_set_property(uint8_t unit_id, uint8_t subunit_id, uint8_t property_id, uint8_t property_value)
    {
        send_command({0x03, unit_id, subunit_id, property_id, property_value});
    }

    void ComfoAirQ::send_command(const std::vector<uint8_t> &command)
    {
        bool is_multi_message_command = command.size() > 8;
        uint32_t can_id = get_command_next_can_id_(local_node_id_, 0x01, 0, is_multi_message_command, false, true);

        if (!is_multi_message_command)
        {
            send_can_message_(can_id, false, command);
            return;
        }

        std::vector<uint8_t> buffer;
        int counter = 0;

        for (auto it = command.begin(); it < command.end(); it += 7)
        {
            bool last = (command.end() - it <= 7);
            buffer.clear();
            buffer.push_back(counter | (last ? 0x80 : 0));
            buffer.insert(buffer.end(), it, std::min(it + 7, command.end()));
            send_can_message_(can_id, false, buffer);
            counter++;
        }
    }

    uint32_t ComfoAirQ::get_command_next_can_id_(
        uint8_t src,
        uint8_t dst,
        uint8_t counter,
        bool is_multi_message_command,
        bool error,
        bool request)
    {
        return get_command_can_id_(src, dst, counter, is_multi_message_command, error, request, get_command_next_sequence_number_());
    }

    uint32_t ComfoAirQ::get_command_can_id_(
        uint8_t src,
        uint8_t dst,
        uint8_t counter,
        bool is_multi,
        bool error,
        bool request,
        uint8_t seq)
    {
        return (0x1F << 24) |
               (seq << 17) |
               (request ? 1 << 16 : 0) |
               (error ? 1 << 15 : 0) |
               (is_multi ? 1 << 14 : 0) |
               (counter << 12) |
               (dst << 6) |
               src;
    }

    uint8_t ComfoAirQ::get_command_next_sequence_number_()
    {
        return command_sequence_number_ = (command_sequence_number_ + 1) & 0x03;
    }

    void ComfoAirQ::send_can_message_(
        uint32_t can_id,
        bool remote_transmission_request,
        const std::vector<uint8_t> &data)
    {
        ESP_LOGD(TAG, "Send CAN: id=0x%08lx (pdo_id=%ld), rtr=%d, size=%d, data=%s",
                 can_id, can_id >> 14, remote_transmission_request, data.size(), format_hex_pretty(data).c_str());

        if (!parent_)
        {
            ESP_LOGE(TAG, "Parent CAN bus not set.");
            return;
        }

        CanbusSendAction<> action;
        action.set_parent(parent_);
        action.set_can_id(can_id);
        if (remote_transmission_request)
        {
            action.set_remote_transmission_request(true);
        }
        action.set_data_static(data);
        action.play();
    }
}
