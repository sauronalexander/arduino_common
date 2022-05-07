#pragma once

#include <utility>
#include <vector>

namespace common::math {

double Interpolate(const std::vector<std::pair<double, double>> &data,
                   double val);

// PROGMEM Math
float PROGMEMSysIdInterpolate(const float *const data, size_t len, double val);

} // namespace common::math
