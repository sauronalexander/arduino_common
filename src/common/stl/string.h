#pragma once

#include <string>

namespace std {

// numeric conversions
int stoi(const string &str, size_t *idx = nullptr, int base = 10);

long stol(const string &str, size_t *idx = nullptr, int base = 10);

unsigned long stoul(const string &str, size_t *idx = nullptr, int base = 10);

//long long stoll(const string &str, size_t *idx = nullptr, int base = 10);

//unsigned long long stoull(const string &str, size_t *idx = nullptr,
//                          int base = 10);

//float stof(const string &str, size_t *idx = nullptr);

double stod(const string &str, size_t *idx = nullptr);

//long double stold(const string &str, size_t *idx = nullptr);

string to_string(int val);

string to_string(unsigned val);

string to_string(long val);

string to_string(unsigned long val);

string to_string(long long val);

string to_string(unsigned long long val);

string to_string(float val);

string to_string(double val);

string to_string(long double val);

#ifdef __UCLIBCXX_HAS_WCHAR__

int stoi(const wstring &str, size_t *idx = nullptr, int base = 10);

long stol(const wstring &str, size_t *idx = nullptr, int base = 10);

unsigned long stoul(const wstring &str, size_t *idx = nullptr, int base = 10);

long long stoll(const wstring &str, size_t *idx = nullptr, int base = 10);

unsigned long long stoull(const wstring &str, size_t *idx = nullptr,
                          int base = 10);

float stof(const wstring &str, size_t *idx = nullptr);

double stod(const wstring &str, size_t *idx = nullptr);

long double stold(const wstring &str, size_t *idx = nullptr);

wstring to_wstring(int val);

wstring to_wstring(unsigned val);

wstring to_wstring(long val);

wstring to_wstring(unsigned long val);

wstring to_wstring(long long val);

wstring to_wstring(unsigned long long val);

wstring to_wstring(float val);

wstring to_wstring(double val);

wstring to_wstring(long double val);
#endif

} // namespace std
