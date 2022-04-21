#pragma once

#include <ArxTypeTraits.h>

namespace common {

template <typename T>
typename std::remove_reference<T>::type&& move(T&& arg) {
  return static_cast<typename std::remove_reference<T>::type&&>(arg);
}

template <typename T>
T *AddressOf(T& obj) {
  return &obj;
}

}  // namespace common
