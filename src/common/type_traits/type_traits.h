#pragma once

#include <type_traits>
#include <ArxTypeTraits.h>
#include <initializer_list>

namespace common::type_traits {

template <typename T>
std::string NameOf() {
  std::string res(
      __PRETTY_FUNCTION__ +
      sizeof "std::string common::type_traits::NameOf() [with T =");
  auto pos = res.find_last_of(';');
  return res.substr(0, pos);
}

template <typename T>
const std::string NameOf(const T& item) {
  return NameOf<T>();
}

template <typename T>
std::string PrettyNameOf() {
  std::string res = NameOf<T>();
  auto pos = res.find_last_of("::");
  return pos == std::string::npos? res : res.substr(pos + 1);
}

template <typename T>
std::string PrettyNameOf(const T& item) {
  return PrettyNameOf<T>();
}

namespace detail {
template <class...>
using void_t = void;

template <class, class T, class... Args>
struct is_constructible_impl : std::false_type {};

template <class T, class... Args>
struct is_constructible_impl<
    void_t<decltype(T(std::declval<Args>()...))>,
    T, Args...> : std::true_type {};

}  // namespace detail

template <class T, class... Args>
using is_constructible =
    detail::is_constructible_impl<detail::void_t<>, T, Args...>;

template <typename ...Args>
struct VariadicTypeTraits {
  static constexpr size_t N = sizeof...(Args);

  template <typename T>
  static constexpr bool Has() {
    return GetIndex<T>() < N;
  }

  template <typename T>
  static constexpr size_t GetIndex() {
    return GetIndexImpl<T, 0, Args...>();
  }

private:
  template <typename T, size_t Index>
  static constexpr size_t GetIndexImpl() {
    return std::numeric_limits<size_t>::max();
  }

  template <typename T, size_t Index, typename T0, typename ...TN>
  static constexpr size_t GetIndexImpl() {
    if constexpr (std::is_same<std::decay_t<T>, std::decay_t<T0>>::value) {
      return Index;
    }
    return GetIndexImpl<T, Index + 1, TN...>();
  }

  template <size_t Index, typename... Types>
  struct GetElementTypeImpl {};

  template <size_t Index, typename T0, typename... TN>
  struct GetElementTypeImpl<Index, T0, TN...>
      : GetElementTypeImpl<Index - 1, TN...> {};

  template <typename T0, typename... TN>
  struct GetElementTypeImpl<0, T0, TN...> {
    using type = T0;
  };

public:
  template <size_t Index>
  using type = typename GetElementTypeImpl<Index, Args...>::type;
};

// TODO: move to unit test
using VariantTestType =
    common::type_traits::VariadicTypeTraits<int, bool, std::string>;
static_assert(VariantTestType::template GetIndex<int>() == 0, "index is 0");
static_assert(VariantTestType::template Has<int>(), "doesn't have int");
static_assert(!VariantTestType::template Has<double>(), "has double");
static_assert(std::is_same<VariantTestType::type<0>, int>::value,
              "the first is not int");
static_assert(std::is_same<VariantTestType::type<2>, std::string>::value,
              "the last is not std::string");


}  // namespace common::type_traits
