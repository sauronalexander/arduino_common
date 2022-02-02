#pragma once

#include "climits"

namespace std {

enum float_round_style {
  round_indeterminate = -1,
  round_toward_zero = 0,
  round_to_nearest = 1,
  round_toward_infinity = 2,
  round_toward_neg_infinity = 3
};

template<int bitsize>
struct __bits_to_base_10 {
  static constexpr int size = -1;
};
template<>
struct __bits_to_base_10<7> {
  static constexpr int size = 2;
};
template<>
struct __bits_to_base_10<8> {
  static constexpr int size = 2;
};
template<>
struct __bits_to_base_10<9> {
  static constexpr int size = 2;
};
template<>
struct __bits_to_base_10<10> {
  static constexpr int size = 3;
};
template<>
struct __bits_to_base_10<15> {
  static constexpr int size = 4;
};
template<>
struct __bits_to_base_10<16> {
  static constexpr int size = 4;
};
template<>
struct __bits_to_base_10<17> {
  static constexpr int size = 5;
};
template<>
struct __bits_to_base_10<18> {
  static constexpr int size = 5;
};
template<>
struct __bits_to_base_10<31> {
  static constexpr int size = 9;
};
template<>
struct __bits_to_base_10<32> {
  static constexpr int size = 9;
};
template<>
struct __bits_to_base_10<35> {
  static constexpr int size = 10;
};
template<>
struct __bits_to_base_10<36> {
  static constexpr int size = 10;
};
template<>
struct __bits_to_base_10<63> {
  static constexpr int size = 18;
};
template<>
struct __bits_to_base_10<64> {
  static constexpr int size = 19;
};
template<>
struct __bits_to_base_10<71> {
  static constexpr int size = 21;
};
template<>
struct __bits_to_base_10<72> {
  static constexpr int size = 21;
};
template<>
struct __bits_to_base_10<79> {
  static constexpr int size = 23;
};
template<>
struct __bits_to_base_10<80> {
  static constexpr int size = 24;
};
template<>
struct __bits_to_base_10<127> {
  static constexpr int size = 38;
};
template<>
struct __bits_to_base_10<128> {
  static constexpr int size = 38;
};


template<class T>
class numeric_limits {
public:
  // General -- meaningful for all specializations.

  static constexpr bool is_specialized = false;
//  static T min();
//  static T max();
//  static constexpr int radix;
//  static constexpr int digits;
//  static constexpr int digits10;
//  static constexpr bool is_signed;
//  static constexpr bool is_integer;
//  static constexpr bool is_exact;
//  static constexpr bool traps;
//  static constexpr bool is_modulo;
//  static constexpr bool is_bounded;
//
//  // Floating point specific.
//
//  static T epsilon();
//  static T round_error();
//  static constexpr int min_exponent10;
//  static constexpr int max_exponent10;
//  static constexpr int min_exponent;
//
//  static constexpr int max_exponent;
//  static constexpr bool has_infinity;
//  static constexpr bool has_quiet_NaN;
//  static constexpr bool has_signaling_NaN;
//  static constexpr bool is_iec559;
//  static constexpr bool has_denorm;
//  static constexpr bool tinyness_before;
//  static constexpr float_round_style round_style;
//  static T denorm_min();
//  static T infinity();
//  static T quiet_NaN();
//  static T signaling_NaN();
};

template<>
class numeric_limits<bool> {
public:
  typedef bool T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return false;
  }

  static T max() {
    return true;
  }

  static constexpr int radix = 2;
  static constexpr int digits = 1;
  static constexpr int digits10 = 0;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = false;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<unsigned char> {
public:
  typedef unsigned char T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return 0;
  }

  static T max() {
    return UCHAR_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT;
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<signed char> {
public:
  typedef signed char T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return SCHAR_MIN;
  }

  static T max() {
    return SCHAR_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT - 1;
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<char> {
public:
  typedef char T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return CHAR_MIN;
  }

  static T max() {
    return CHAR_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = (CHAR_MIN != 0) ? CHAR_BIT - 1 : CHAR_BIT;
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = (CHAR_MIN != 0);
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<unsigned short> {
public:
  typedef unsigned short T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return 0;
  }

  static T max() {
    return USHRT_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT * sizeof(T);
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<signed short> {
public:
  typedef signed short T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return SHRT_MIN;
  }

  static T max() {
    return SHRT_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT * sizeof(T);
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<unsigned int> {
public:
  typedef unsigned int T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return 0;
  }

  static T max() {
    return UINT_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT * sizeof(T);
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<signed int> {
public:
  typedef signed int T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return INT_MIN;
  }

  static T max() {
    return INT_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT * sizeof(T);
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<unsigned long int> {
public:
  typedef unsigned long int T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return 0;
  }

  static T max() {
    return ULONG_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT * sizeof(T);
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = false;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<signed long int> {
public:
  typedef signed long int T;
  // General -- meaningful for all specializations.
  static constexpr bool is_specialized = true;

  static T min() {
    return LONG_MIN;
  }

  static T max() {
    return LONG_MAX;
  }

  static constexpr int radix = 2;
  static constexpr int digits = CHAR_BIT * sizeof(T);
  static constexpr int digits10 = __bits_to_base_10<digits>::size;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = true;
  static constexpr bool is_exact = true;
  static constexpr bool traps = false;
  static constexpr bool is_modulo = true;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static T epsilon() {
    return 0;
  }

  static T round_error() {
    return 0;
  }

  static constexpr int min_exponent10 = 0;
  static constexpr int max_exponent10 = 0;
  static constexpr int min_exponent = 0;

  static constexpr int max_exponent = 0;
  static constexpr bool has_infinity = false;
  static constexpr bool has_quiet_NaN = false;
  static constexpr bool has_signaling_NaN = false;
  static constexpr bool is_iec559 = false;
  static constexpr bool has_denorm = false;
  static constexpr bool tinyness_before = false;
  static constexpr float_round_style round_style = round_indeterminate;

  static T denorm_min();

  static T infinity();

  static T quiet_NaN();

  static T signaling_NaN();
};

template<>
class numeric_limits<double> {
public:
  typedef double numeric_type;

  static constexpr bool is_specialized = true;

  static numeric_type min() { return __DBL_MIN__; }

  static numeric_type max() { return __DBL_MAX__; }

  static constexpr int radix = __FLT_RADIX__;
  static constexpr int digits = __DBL_MANT_DIG__;
  static constexpr int digits10 = __DBL_DIG__;
  static constexpr bool is_signed = true;
  static constexpr bool is_integer = false;
  static constexpr bool is_exact = false;
  static constexpr bool traps = false; // this is a guess
  static constexpr bool is_modulo = false;
  static constexpr bool is_bounded = true;

  // Floating point specific.

  static numeric_type epsilon() { return __DBL_EPSILON__; }

  static numeric_type round_error() { return 0.5; }

  static constexpr int min_exponent10 = -1; //How do I properly get this?
  static constexpr int max_exponent10 = -1; //How do I properly get this?
  static constexpr int min_exponent = -1; //How do I properly get this?
  static constexpr int max_exponent = -1; //How do I properly get this?
  static constexpr bool has_infinity = false; //I don't know, so until I can find out, I'm saying no
  static constexpr bool has_quiet_NaN = false; //I don't know, so until I can find out, I'm saying no
  static constexpr bool has_signaling_NaN = false; //I don't know, so until I can find out, I'm saying no
  static constexpr bool has_denorm = false; //I don't know, so until I can find out, I'm saying no

  static constexpr bool is_iec559 = false;  //I don't know, so until I can find out, I'm saying no
  static constexpr bool tinyness_before = false; // more questions
  static constexpr float_round_style round_style = round_to_nearest; // more questions
  static numeric_type denorm_min() { return -1; } //How do I properly get this?
  static numeric_type infinity() { return -1; } //How do I properly get this?
  static numeric_type quiet_NaN() { return -1; } //How do I properly get this?
  static numeric_type signaling_NaN() { return -1; } //How do I properly get this?
};


}

#pragma GCC visibility pop

