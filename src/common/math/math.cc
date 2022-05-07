#include "common/math/math.h"

#include <algorithm>
#include <avr/pgmspace.h>

namespace common::math {

double Interpolate(const std::vector<std::pair<double, double>> &data,
                   double x) {
  if (x <= data.front().first) {
    return data.front().second;
  }
  if (x >= data.back().first) {
    return data.back().second;
  }
  auto it = std::upper_bound(
      data.begin(), data.end(), x,
      [](double val, const std::pair<double, double> &item) -> bool {
        return val < item.first;
      });
  double k = (it->second - (it - 1)->second) / (it->first - (it - 1)->first);
  return (x - (it - 1)->first) * k + (it - 1)->second;
}

float PROGMEMSysIdInterpolate(const float *const data, size_t len, double val) {
  if (len <= 1) {
    return 0.0;
  }
  if (val <= pgm_read_float(data)) {
    return pgm_read_float(data + 1);
  }
  size_t num_row = len / 2;
  if (val >= pgm_read_float(data + len - 2)) {
    return pgm_read_float(data + len - 1);
  }
  size_t count, step, front;
  count = num_row;
  front = 0;
  while (count > 0) {
    auto it = front;
    step = count / 2;
    it += step;
    if (val >= pgm_read_float(data + 2 * it)) {
      front = it + 1;
      count -= step + 1;
    } else {
      count = step;
    }
  }
  return pgm_read_float(data + 2 * front + 1);
}

} // namespace common::math