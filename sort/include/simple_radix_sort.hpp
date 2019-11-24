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
//MSD
namespace rdx {
namespace {}
static inline void simple_radix_sort_seq(uint32_t* start, uint32_t* end) {
  const size_t count = end - start;
  uint32_t* sorted = new uint32_t[count];
  uint8_t* key_cache = new uint8_t[count];

  for(auto depth = 0; depth < sizeof(uint32_t)/sizeof(uint8_t); ++depth){
    uint8_t bucket_size[256]{0};
    for(auto i = 0; i < count; ++i){
      key_cache[i] = ((uint8_t*)(start + i))[depth];   
      ++bucket_size[key_cache[i]];
    }


    uint32_t* bucket[256];
    bucket[0] = sorted;
    for(auto i = 1; i < 256; ++i){
      bucket[i] = bucket[i-1] + bucket_size[i-1];
    }

    for(auto i = 0; i < count; ++i){
      *(bucket[key_cache[i]]++) = *(start + i);
    }
    std::copy(sorted, sorted + count, start);
  }


  delete[] key_cache;
  delete[] sorted;
}
  
}  // namespace rdx
