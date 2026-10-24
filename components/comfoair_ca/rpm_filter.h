#pragma once

#include <array>
#include <cstdint>
#include <cstddef>

namespace esphome
{
  namespace comfoair_ca
  {

    constexpr std::size_t RPM_FILTER_WINDOW{3};

    class MedianFilter3
    {
    public:
      bool update(float sample, float &filtered)
      {
        samples_[next_] = sample;
        next_ = (next_ + 1) % RPM_FILTER_WINDOW;
        if (filled_ < RPM_FILTER_WINDOW)
        {
          ++filled_;
        }
        if (!ready())
        {
          return false;
        }
        filtered = median();
        return true;
      }

      void reset()
      {
        filled_ = 0;
        next_ = 0;
      }

      bool ready() const { return filled_ >= RPM_FILTER_WINDOW; }

    private:
      float median() const
      {
        std::array<float, RPM_FILTER_WINDOW> sorted{samples_};
        if (sorted[1] < sorted[0])
        {
          const float tmp{sorted[0]};
          sorted[0] = sorted[1];
          sorted[1] = tmp;
        }
        if (sorted[2] < sorted[1])
        {
          const float tmp{sorted[1]};
          sorted[1] = sorted[2];
          sorted[2] = tmp;
        }
        if (sorted[1] < sorted[0])
        {
          const float tmp{sorted[0]};
          sorted[0] = sorted[1];
          sorted[1] = tmp;
        }
        return sorted[1];
      }

      std::array<float, RPM_FILTER_WINDOW> samples_{};
      std::size_t next_{0};
      std::size_t filled_{0};
    };

  } // namespace comfoair_ca
} // namespace esphome
