""" Creates module ComfoAir Q """

import esphome.codegen as cg
import esphome.config_validation as cv
from esphome.const import CONF_ID
from esphome.components.canbus import CONF_CANBUS_ID, CanbusComponent, CanSpeed

DEPENDENCIES = ["canbus"]

zehnder_comfoair_q_ns = cg.esphome_ns.namespace("comfoair_q")
ZehnderComfoAirQ = zehnder_comfoair_q_ns.class_("ComfoAirQ", cg.Component)

# Custom options for the ComfoAir Q component
CONF_REQUEST_INTERVAL = "request_interval"
CONF_REQUEST_IDS = "request_ids"
CONF_REQUEST_DELAY = "request_delay"
CONF_REQUEST_LOCAL_NODE_ID = "local_node_id"

CONFIG_SCHEMA = cv.Schema(
    {
        cv.GenerateID(): cv.declare_id(ZehnderComfoAirQ),
        cv.GenerateID(CONF_CANBUS_ID): cv.use_id(CanbusComponent),
        cv.Optional(CONF_REQUEST_INTERVAL, default="1h"): cv.update_interval,
        cv.Optional(CONF_REQUEST_IDS, default=[]): cv.ensure_list(
            cv.int_range(min=0, max=0x3FF)
        ),
        cv.Optional(CONF_REQUEST_DELAY, default="100ms"): cv.positive_time_period_milliseconds,
        cv.Optional(CONF_REQUEST_LOCAL_NODE_ID, default=0x2A): cv.int_range(min=0x01, max=0x3F),
    }
)


async def to_code(config):
    """ Generate C++ code for the ComfoAir Q component. """
    parent = await cg.get_variable(config[CONF_CANBUS_ID])
    cg.add(parent.set_use_extended_id(True))
    cg.add(parent.set_bitrate(CanSpeed.CAN_50KBPS))

    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)

    cg.add(var.set_parent(parent))
    cg.add(var.set_update_interval(config[CONF_REQUEST_INTERVAL]))
    cg.add(var.set_request_ids(config[CONF_REQUEST_IDS]))
    cg.add(var.set_request_delay(config[CONF_REQUEST_DELAY]))
    cg.add(var.set_local_node_id(config[CONF_REQUEST_LOCAL_NODE_ID]))
