#pragma once
#include <algorithm>
#include <array>
#include <cstdint>
#include <cstring>
#include <functional>
#include <memory>

#include <cassert>
#include <iostream>
#include <type_traits>
#include <typeinfo>

namespace rdx {
namespace {}
template <typename Iterator, typename Comparator>
static inline void radix_sort(const Iterator begin, const Iterator end,
                              const Comparator comp) {
  typedef typename std::iterator_traits<Iterator>::value_type KeyType;
  constexpr const size_t window_size = 1 << 2;
  constexpr const size_t window_count = (sizeof(KeyType) * 8) / window_size;
  const size_t element_count = (end - begin);

  std::cout << "window_count:" << window_count << " window_size:" << window_size
            << " element_count:" << element_count << '\n';

  std::sort(begin, end, comp);
}

template <typename Iterator, typename KeyGetter>
static inline void radix_sort_seq(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  //Start of actual work
  std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);

  for (size_t depth = 0; depth < size_of_key; ++depth) {
    std::array<size_t, 256> bucket_size{0};  // Init to 0
    // Read bytes and count occurances
    for (size_t i = 0; i < element_count; ++i) {
			auto key = key_getter(*(begin + i));
      key_cache[i] = reinterpret_cast<char*>(&key)[depth];
			++bucket_size[key_cache[i]];
    }

		//Prefix sum
		std::array<data_type*, 256> bucket;
		bucket[0] = data_cache.get();
		for(size_t i = 1; i < 256; ++i){
			bucket[i] = bucket[i - 1] + bucket_size[i - 1];	
		}	

		for(size_t i = 0; i < element_count; ++i){
			*(bucket[key_cache[i]]++) = *(begin + i);
		}	
    std::copy(data_cache.get(), data_cache.get() + element_count, begin);
  }
  // TODO CONTINUE
}

}  // namespace rdx
