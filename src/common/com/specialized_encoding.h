#pragma once

#include "common/stl/string.h"
#include "common/type_traits/type_traits.h"

namespace common::com {

// TODO: Add specialized encoding template.

template<typename Type,
    typename = std::enable_if_t<std::is_arithmetic<Type>::value>>
void Encode(const Type &num, std::string &data) {
  const char *ptr = (const char *) &num;
  data.resize(sizeof(num));
  for (uint32_t i{0}; i < sizeof(num); i++) {
    data[i] = ptr[i];
  }
}

template<typename Type,
    typename = std::enable_if_t<std::is_arithmetic<Type>::value>>
void Decode(const std::string &data, Type &num) {
  char *ptr = (char *) &num;
  for (uint32_t i{0}; i < sizeof(num); i++) {
    ptr[i] = data[i];
  }
}

} // namespace common::com
