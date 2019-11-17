#pragma once
#include <algorithm>
#include <cstdint>
#include <cstring>
#include <functional>
namespace rdx{
template <typename Iterator, typename Comparator>
static inline void radix_sort(Iterator begin, Iterator end, Comparator comp, size_t MaxDepth) {
  //typedef std::less<typename std::iterator_traits<Iterator>::value_type> Comparator;
  std::sort(begin, end, comp);
  (void) MaxDepth;
}
}
