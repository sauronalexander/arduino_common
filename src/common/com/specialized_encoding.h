#pragma once

#include "common/stl/string.h"
#include "common/type_traits/type_traits.h"

namespace common::com {

template <typename T>
class IsEncodible {
private:
  template<typename MsgType>
  static std::false_type HasEncodingMethodImpl(...);

  template<typename MsgType,
      typename = decltype(&MsgType::Encode),
      typename = decltype(&MsgType::Decode)>
  static std::true_type HasEncodingMethodImpl(int);

  template<typename MsgType>
  static std::false_type HasEncodingFunctionImpl(...);

  template<typename MsgType,
      typename =
      decltype(StringTranslate(std::declval<const MsgType &>(),
                               std::declval<std::string &>())),
      typename =
      decltype(StringTranslate(std::declval<const std::string &>(),
                               std::declval<MsgType &>()))>
  static std::true_type HasEncodingFunctionImpl(int);

public:
  enum { has_encoding_function =
    decltype(HasEncodingFunctionImpl<std::decay_t<T>>(0))::value };
  enum { has_encoding_method =
      decltype(HasEncodingMethodImpl<std::decay_t<T>>(0))::value };
  enum { value =
    has_encoding_function || has_encoding_method };
};

void Encode(...);
void Decode(...);

template <typename Type,
    typename = std::enable_if_t<IsEncodible<Type>::has_encoding_function>>
inline void Encode(const Type &source, std::string &data) {
  StringTranslate(source, data);
}

template <typename Type,
    typename = std::enable_if_t<IsEncodible<Type>::has_encoding_function>>
inline void Decode(const std::string &data, Type &source) {
  StringTranslate(data, source);
}

template <typename Type,
    typename = std::enable_if_t<IsEncodible<Type>::has_encoding_method>,
    typename = std::nullptr_t>
inline void Encode(const Type &source, std::string &data) {
  source.Encode(data);
}

template <typename Type,
    typename = std::enable_if_t<IsEncodible<Type>::has_encoding_method>,
    typename = std::nullptr_t>
inline void Decode(const std::string &data, Type &source) {
  source.Decode(data);
}

} // namespace common::com

// All specialized encoding methods should be put in namespace std for ADL.
namespace std {

template<typename Type,
    typename = std::enable_if_t<std::is_arithmetic<Type>::value>>
inline void StringTranslate(const Type &num, std::string &data) {
  const char *ptr = (const char *) &num;
  data.resize(sizeof(num));
  for (uint32_t i{0}; i < sizeof(num); i++) {
    data[i] = ptr[i];
  }
}

template<typename Type,
    typename = std::enable_if_t<std::is_arithmetic<Type>::value>>
inline void StringTranslate(const std::string &data, Type &num) {
  char *ptr = (char *) &num;
  for (uint32_t i{0}; i < sizeof(num); i++) {
    ptr[i] = data[i];
  }
}

}  // namespace std
