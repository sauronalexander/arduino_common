#pragma once

#include <type_traits>
#include <ArxTypeTraits.h>
#include <initializer_list>

namespace common::type_traits {

template <typename ...Args>
struct VariadicTypeTraits {
  static constexpr size_t N = sizeof...(Args);

  template <typename T>
  static constexpr bool Has() {
    auto list = {arx::stdx::is_same<arx::stdx::decay_t<T>, Args>::value...};
    for (auto ele : list) {
      if (ele) {
        return true;
      }
    }
    return false;
  }
};

}  // namespace common::type_traits
