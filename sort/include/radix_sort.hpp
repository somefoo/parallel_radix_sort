#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>

#include <iostream>
#include <cassert>
#include <type_traits>
#include <typeinfo>

namespace rdx{
  namespace {
     
  }
template <typename Iterator, typename Comparator>
static inline void radix_sort(const Iterator begin, const Iterator end, const Comparator comp) {
  typedef typename std::iterator_traits<Iterator>::value_type Type;
  constexpr const size_t window_size = 1 << 2;
  constexpr const size_t window_count = (sizeof(Type) * 8) / window_size;
  const size_t element_count = (end - begin);

  std::cout << "window_count:" << window_count << " window_size:" << window_size << " element_count:" << element_count << '\n';

  std::sort(begin, end, comp);
}
}
