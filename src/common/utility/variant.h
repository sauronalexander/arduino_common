#pragma once

#include "common/type_traits/type_traits.h"

namespace common {

#define VARIANT_USE_BINARY_SEARCH

namespace detail {

#ifdef VARIANT_USE_BINARY_SEARCH
template <size_t StartIndex, size_t EndIndex, typename ...Types,
    typename = std::enable_if_t<!(StartIndex <= EndIndex)>>
bool Destroy(...) {
  return false;
}

template <size_t StartIndex, size_t EndIndex, typename ...Types,
          typename = std::enable_if_t<StartIndex <= EndIndex>,
          typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
          typename CurType = typename TypeList::template type<StartIndex>>
bool Destroy(size_t index, void *data) {
  constexpr size_t mid = (StartIndex + EndIndex) / 2;
  if (mid == index) {
    auto ptr = reinterpret_cast<CurType *>(data);
    delete ptr;
    return true;
  }
  if constexpr (mid == StartIndex) {
    return Destroy<StartIndex + 1, EndIndex, Types...>(index, data);
  }
  return index > mid ? Destroy<mid, EndIndex, Types...>(index, data) :
                       Destroy<StartIndex, mid, Types...>(index, data);
}

template <size_t StartIndex, size_t EndIndex, typename ...Types,
    typename = std::enable_if_t<!(StartIndex <= EndIndex)>>
void *Copy(...) {
  return nullptr;
}

template <size_t StartIndex, size_t EndIndex, typename ...Types,
    typename = std::enable_if_t<StartIndex <= EndIndex>,
    typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
    typename CurType = typename TypeList::template type<StartIndex>>
void *Copy(size_t index, const void *data) {
  constexpr size_t mid = (StartIndex + EndIndex) / 2;
  if (mid == index) {
    return reinterpret_cast<void *>(
        new CurType{*reinterpret_cast<const CurType *>(data)});
  }
  if constexpr (mid == StartIndex) {
    return Copy<StartIndex + 1, EndIndex, Types...>(index, data);
  }
  return index > mid ? Copy<mid, EndIndex, Types...>(index, data) :
         Copy<StartIndex, mid, Types...>(index, data);
}

#else
template <size_t Index, typename ...Types,
    typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
    typename = std::enable_if_t<Index >= TypeList::N>>
bool Destroy(...) {
  return false;
}

template <size_t Index, typename ...Types,
          typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
          typename = std::enable_if_t<Index < TypeList::N>>
bool Destroy(size_t index, void *data) {
  using CurType = typename TypeList::template type<Index>;
  if (data && Index == index) {
    auto ptr = reinterpret_cast<CurType *>(data);
    delete ptr;
    return true;
  }
  return Destroy<Index + 1, Types...>(index, data);
}

template <size_t Index, typename ...Types,
    typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
    typename = std::enable_if_t<Index >= TypeList::N>>
void *Copy(...) {
  return false;
}

template <size_t Index, typename ...Types,
          typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
          typename = std::enable_if_t<Index < TypeList::N>>
void *Copy(size_t index, const void *data) {
  using CurType = typename TypeList::template type<Index>;
  if (data && Index == index) {
    return reinterpret_cast<void *>(
        new CurType{*reinterpret_cast<const CurType *>(data)});
    return ptr;
  }
  return Destroy<Index + 1, Types...>(index, data);
}
#endif

template <size_t Index, typename... Args>
void *Construct(Args&&... args) {
  return nullptr;
}

template <size_t Index, typename... Types, typename... Args,
          typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
          typename CurType = typename TypeList::template type<Index>,
          typename = std::enable_if_t<
              common::type_traits::is_constructible<CurType, Args...>::value>>
void *Construct(size_t &index, Args&&... args) {
  index = Index;
  return reinterpret_cast<void *>(new CurType{std::forward<Args>(args)...});
}

template <size_t Index,  typename... Types, typename... Args,
    typename TypeList = common::type_traits::VariadicTypeTraits<Types...>,
    typename CurType = typename TypeList::template type<Index>,
    typename = std::enable_if_t<
        !common::type_traits::is_constructible<CurType, Args...>::value>,
    typename = std::nullptr_t>
void *Construct(size_t &index, Args&&... args) {
  return Construct<Index + 1, Args..., Types...>(
      index, std::forward<Args>(args)...);
}

}  //namespace detail.

// Simplified verison of std::variant
template <class... Types>
class Variant {
private:
  using TypeList = common::type_traits::VariadicTypeTraits<Types...>;

public:
  Variant() = default;

  Variant(const Variant &other)  {
    *this = other;
  }

  Variant(Variant &&other) {
    *this = std::move(other);
  }

  Variant &operator=(const Variant &other) {
#ifdef VARIANT_USE_BINARY_SEARCH
    data_ = detail::Copy<0, TypeList::N - 1, Types...>(
        other.index_, other.data_);
#else
    data_ = detail::Copy<0, Types...>(other.index_, other.data_);
#endif
    if (data_) {
      index_ = other.index_;
    }
    return *this;
  };

  Variant &operator=(Variant &&other) {
    this->~Variant();
    data_ = other.data_;
    index_ = other.index_;
    other.data_ = nullptr;
    other.index_ = std::numeric_limits<size_t>::max();
    return *this;
  };

  ~Variant() {
    if (index_ < TypeList::N && data_) {
#ifdef VARIANT_USE_BINARY_SEARCH
      detail::Destroy<0, TypeList::N - 1, Types...>(index_, data_);
#else
      detail::Destroy<0, Types...>(index_, data_);
#endif
      index_ = std::numeric_limits<size_t>::max();
      data_ = nullptr;
    }
  }

  template <
      typename T,
      size_t Index = TypeList::template GetIndex<T>(),
      typename = std::enable_if_t<TypeList::template Has<T>()>>
  explicit Variant(T&& arg) {
    index_ = Index;
    data_ = reinterpret_cast<void *>(
        new typename TypeList::template type<Index>{std::forward<T>(arg)});
  }

  template <typename... Args>
  explicit Variant(Args&&... args) {
    data_ = detail::Construct<0, Types...>(index_, std::forward<Args>(args)...);
  }

  constexpr size_t Index() const noexcept {
    return index_;
  }


  template <typename T, size_t Index = TypeList::template GetIndex<T>()>
  bool HoldsAlternative() const {
    return index_ == Index;
  }

  template <typename T, size_t Index = TypeList::template GetIndex<T>()>
  T *GetIf() {
    return index_ == Index ? reinterpret_cast<T *>(data_) : nullptr;
  }

  template <typename T, size_t Index = TypeList::template GetIndex<T>()>
  const T *GetIf() const {
    return index_ == Index ? reinterpret_cast<T *>(data_) : nullptr;
  }

  template <typename T, typename... Args,
            typename = std::enable_if_t<
                common::type_traits::is_constructible<T, Args...>::value>,
            size_t Index = TypeList::template GetIndex<T>()>
  void Emplace(Args&&... args) {
    if (data_) {
      this->~Variant();
    }
    index_ = Index;
    data_ = reinterpret_cast<void *>(
        new typename TypeList::template type<Index>(
            std::forward<Args>(args)...));
  }

private:
  static_assert(TypeList::N > 0, "must have at least 1 type");

  void *data_{nullptr};
  size_t index_ = std::numeric_limits<size_t>::max();
};

}  // namespace common
