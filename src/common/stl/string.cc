#include "common/stl/string.h"


#include "cerrno"
#include "stdlib.h"
#include <stdio.h>
#include <cmath>

#include "common/stl/limits.h"

//#include <ArxTypeTraits.h>

namespace std {

template string operator+
    <char, char_traits<char>, allocator<char>>(char const *, string const &);

namespace {

// as_integer
template<typename V, typename S, typename F>
inline V as_integer_helper(const string &func, const S &str, size_t *idx,
                           int base, F f) {
  typename S::value_type *ptr = nullptr;
  const typename S::value_type *const p = str.c_str();
//  typename remove_reference<decltype(errno)>::type errno_save = errno;
//  errno = 0;
  V r = f(p, &ptr, base);
  //  swap(errno, errno_save);
  //  if (errno_save == ERANGE)
  //    throw_from_string_out_of_range(func);
  //  if (ptr == p)
  //    throw_from_string_invalid_arg(func);
  if (idx)
    *idx = static_cast<size_t>(ptr - p);
  return r;
}

template<typename V, typename S>
inline V as_integer(const string &func, const S &s, size_t *idx, int base);

// string
template<>
inline int as_integer(const string &func, const string &s, size_t *idx,
                      int base) {
  // Use long as no Standard string to integer exists.
  long r = as_integer_helper<long>(func, s, idx, base, strtol);
  //  if (r < numeric_limits<int>::min() || numeric_limits<int>::max() < r)
  //    throw_from_string_out_of_range(func);
  return static_cast<int>(r);
}

template<>
inline long as_integer(const string &func, const string &s, size_t *idx,
                       int base) {
  return as_integer_helper<long>(func, s, idx, base, strtol);
}

template<>
inline unsigned long as_integer(const string &func, const string &s,
                                size_t *idx, int base) {
  return as_integer_helper<unsigned long>(func, s, idx, base, strtoul);
}

// template<>
// inline long long as_integer(const string &func, const string &s, size_t *idx,
//                             int base) {
//   return as_integer_helper<long long>(func, s, idx, base, strtoll);
// }
//
// template<>
// inline unsigned long long as_integer(const string &func, const string &s,
//                                      size_t *idx, int base) {
//   return as_integer_helper<unsigned long long>(func, s, idx, base, strtoull);
// }

// as_float
template<typename V, typename S, typename F>
inline V as_float_helper(const string &func, const S &str, size_t *idx, F f) {
  typename S::value_type *ptr = nullptr;
  const typename S::value_type *const p = str.c_str();
//  typename remove_reference<decltype(errno)>::type errno_save = errno;
//  errno = 0;
  V r = f(p, &ptr);
  //  swap(errno, errno_save);
  //  if (errno_save == ERANGE)
  //    throw_from_string_out_of_range(func);
  //  if (ptr == p)
  //    throw_from_string_invalid_arg(func);
  if (idx)
    *idx = static_cast<size_t>(ptr - p);
  return r;
}

template<typename V, typename S>
inline V as_float(const string &func, const S &s, size_t *idx = nullptr);

//template <>
//inline float as_float(const string &func, const string &s, size_t *idx) {
//  return as_float_helper<float>(func, s, idx, strtof);
//}

template<>
inline double as_float(const string &func, const string &s, size_t *idx) {
  return as_float_helper<double>(func, s, idx, strtod);
}

//template <>
//inline long double as_float(const string &func, const string &s, size_t *idx) {
//  return as_float_helper<long double>(func, s, idx, strtold);
//}


} // unnamed namespace
int stoi(const string &str, size_t *idx, int base) {
  return as_integer<int>("stoi", str, idx, base);
}

long stol(const string &str, size_t *idx, int base) {
  return as_integer<long>("stol", str, idx, base);
}

unsigned long stoul(const string &str, size_t *idx, int base) {
  return as_integer<unsigned long>("stoul", str, idx, base);
}

//long long stoll(const string &str, size_t *idx, int base) {
//  return as_integer<long long>("stoll", str, idx, base);
//}
//
//long long stoll(const wstring &str, size_t *idx, int base) {
//  return as_integer<long long>("stoll", str, idx, base);
//}
//
//unsigned long long stoull(const string &str, size_t *idx, int base) {
//  return as_integer<unsigned long long>("stoull", str, idx, base);
//}
//
//unsigned long long stoull(const wstring &str, size_t *idx, int base) {
//  return as_integer<unsigned long long>("stoull", str, idx, base);
//}

//float stof(const string &str, size_t *idx) {
//  return as_float<float>("stof", str, idx);
//}

double stod(const string &str, size_t *idx) {
  return as_float<double>("stod", str, idx);
}

//long double stold(const string &str, size_t *idx) {
//  return as_float<long double>("stold", str, idx);
//}

// to_string
namespace {
// as_string
template<typename S, typename P, typename V>
inline S as_string(P sprintf_like, S s, const typename S::value_type *fmt,
                   V a) {
  typedef typename S::size_type size_type;
  size_type available = s.size();
  while (true) {
    int status = sprintf_like(&s[0], available + 1, fmt, a);
    if (status >= 0) {
      size_type used = static_cast<size_type>(status);
      if (used <= available) {
        s.resize(used);
        break;
      }
      available = used; // Assume this is advice of how much space we need.
    } else
      available = available * 2 + 1;
    s.resize(available);
  }
  return s;
}

inline std::string as_string_float(std::string s, double val) {
  static constexpr int kFloatPrecision = 1E4;
  int val_int = (int) val;
  int val_frag = (int) ((val - (double) val_int) * kFloatPrecision);
  if (val_frag < 0) {
    val_frag = -val_frag;
  }
  s = to_string(val_int) + "." + to_string(val_frag);
  return s;
}

inline std::string as_string_float(std::string s, float val) {
  static constexpr int kFloatPrecision = 1E4;
  int val_int = (int) val;
  int val_frag = (int) ((val - (float) val_int) * kFloatPrecision);
  if (val_frag < 0) {
    val_frag = -val_frag;
  }
  s = to_string(val_int) + "." + to_string(val_frag);
  return s;
}

inline std::string as_string_float(std::string s, long double val) {
  static constexpr long long kFloatPrecision = 1E6;
  long long val_int = (long long) val;
  long long val_frag =
      (long long) ((val - (long double) val_int) * kFloatPrecision);
  if (val_frag < 0) {
    val_frag = -val_frag;
  }
  s = to_string(val_int) + "." + to_string(val_frag);
  return s;
}

template<class S>
struct initial_string;

template<>
struct initial_string<string> {
  string operator()() const {
    string s;
    s.resize(s.capacity());
    return s;
  }
};

template<typename S, typename V>
S i_to_string(const V v) {
  //  numeric_limits::digits10 returns value less on 1 than desired for unsigned
  //  numbers. For example, for 1-byte unsigned value digits10 is 2 (999 can not
  //  be represented), so we need +1 here.
  constexpr size_t bufsize =
      numeric_limits<V>::digits10 + 2; // +1 for minus, +1 for digits10
  char buf[bufsize];
  const auto res = to_chars(buf, buf + bufsize, v);
  //  _LIBCPP_ASSERT(res.ec == errc(),
  //                 "bufsize must be large enough to accomodate the value");
  return S(buf, res.ptr);
}
} // unnamed namespace

string to_string(int val) {
  return as_string(snprintf, initial_string<string>()(), "%i", val);
}

string to_string(long val) {
  return as_string(snprintf, initial_string<string>()(), "%i", val);
}

string to_string(long long val) {
  return as_string(snprintf, initial_string<string>()(), "%lld", val);
}

string to_string(unsigned val) {
  return as_string(snprintf, initial_string<string>()(), "%u", val);
}

string to_string(unsigned long val) {
  return as_string(snprintf, initial_string<string>()(), "%lu", val);
}

string to_string(unsigned long long val) {
  return as_string(snprintf, initial_string<string>()(), "%llu", val);
}

string to_string(float val) {
  return as_string_float(initial_string<string>()(), val);
}

string to_string(double val) {
  return as_string_float(initial_string<string>()(), val);
}

string to_string(long double val) {
  return as_string_float(initial_string<string>()(), val);
}

// wstring
#ifdef __UCLIBCXX_HAS_WCHAR__

int stoi(const wstring &str, size_t *idx, int base) {
  return as_integer<int>("stoi", str, idx, base);
}

long stol(const wstring &str, size_t *idx, int base) {
  return as_integer<long>("stol", str, idx, base);
}

unsigned long stoul(const wstring &str, size_t *idx, int base) {
  return as_integer<unsigned long>("stoul", str, idx, base);
}

float stof(const wstring &str, size_t *idx) {
  return as_float<float>("stof", str, idx);
}

double stod(const wstring &str, size_t *idx) {
  return as_float<double>("stod", str, idx);
}

long double stold(const wstring &str, size_t *idx) {
  return as_float<long double>("stold", str, idx);
}

template <>
inline int as_integer(const string &func, const wstring &s, size_t *idx,
                      int base) {
  // Use long as no Stantard string to integer exists.
  long r = as_integer_helper<long>(func, s, idx, base, wcstol);
  //  if (r < numeric_limits<int>::min() || numeric_limits<int>::max() < r)
  //    throw_from_string_out_of_range(func);
  return static_cast<int>(r);
}

template <>
inline long as_integer(const string &func, const wstring &s, size_t *idx,
                       int base) {
  return as_integer_helper<long>(func, s, idx, base, wcstol);
}

template <>
inline unsigned long as_integer(const string &func, const wstring &s,
                                size_t *idx, int base) {
  return as_integer_helper<unsigned long>(func, s, idx, base, wcstoul);
}

template <>
inline long long as_integer(const string &func, const wstring &s, size_t *idx,
                            int base) {
  return as_integer_helper<long long>(func, s, idx, base, wcstoll);
}

template <>
inline unsigned long long as_integer(const string &func, const wstring &s,
                                     size_t *idx, int base) {
  return as_integer_helper<unsigned long long>(func, s, idx, base, wcstoull);
}

template <>
inline float as_float(const string &func, const wstring &s, size_t *idx) {
  return as_float_helper<float>(func, s, idx, wcstof);
}

template <>
inline double as_float(const string &func, const wstring &s, size_t *idx) {
  return as_float_helper<double>(func, s, idx, wcstod);
}

template <>
inline long double as_float(const string &func, const wstring &s, size_t *idx) {
  return as_float_helper<long double>(func, s, idx, wcstold);
}

wstring to_wstring(int val) { return i_to_string<wstring>(val); }
wstring to_wstring(long val) { return i_to_string<wstring>(val); }
wstring to_wstring(long long val) { return i_to_string<wstring>(val); }
wstring to_wstring(unsigned val) { return i_to_string<wstring>(val); }
wstring to_wstring(unsigned long val) { return i_to_string<wstring>(val); }
wstring to_wstring(unsigned long long val) { return i_to_string<wstring>(val); }

wstring to_wstring(float val) {
  return as_string(get_swprintf(), initial_string<wstring>()(), L"%f", val);
}

wstring to_wstring(double val) {
  return as_string(get_swprintf(), initial_string<wstring>()(), L"%f", val);
}

wstring to_wstring(long double val) {
  return as_string(get_swprintf(), initial_string<wstring>()(), L"%Lf", val);
}

#endif

} // namespace std
