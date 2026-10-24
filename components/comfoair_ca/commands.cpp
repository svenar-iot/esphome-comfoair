#include "commands.h"

namespace esphome
{
  namespace comfoair_ca
  {
    void encode_ventilation_levels(const VentilationLevels &levels,
                                   uint8_t (&out)[VENTILATION_LEVEL_PAYLOAD_SIZE]) noexcept
    {
      out[0] = levels.exhaust_absent;
      out[1] = levels.exhaust_low;
      out[2] = levels.exhaust_medium;
      out[3] = levels.supply_absent;
      out[4] = levels.supply_low;
      out[5] = levels.supply_medium;
      out[6] = levels.exhaust_high;
      out[7] = levels.supply_high;
      out[8] = levels.unused;
    }

  }
}
