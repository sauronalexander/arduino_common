#pragma once

#include "common/type_traits/type_traits.h"

namespace common::com {

template<typename Type,
    typename = std::enable_if_t<std::is_arithmetic<Type>::value>>
void Encode(const Type &num, char *data) {
  const char *ptr = (const char *) &num;
  for (uint32_t i{0}; i < sizeof(num); i++) {
    data[i] = ptr[i];
  }
}

template<typename Type,
    typename = std::enable_if_t<std::is_arithmetic<Type>::value>>
void Decode(const char *const data, Type &num) {
  char *ptr = (char *) &num;
  for (uint32_t i{0}; i < sizeof(num); i++) {
    ptr[i] = data[i];
  }
}

} // namespace common::com
