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

#include <omp.h>

#include "debug_helper.hpp"

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

  std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);


  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);

  //Start of actual work
  for (size_t depth = 0; depth < size_of_key; ++depth) {
    std::array<size_t, 256> bucket_size{0};  // Init to 0
    // Read bytes and count occurances
    for (size_t i = 0; i < element_count; ++i) {
			auto key = key_getter(*(begin_original + i));
      //TODO WARNING ENDIANESS!
      key_cache[i] = reinterpret_cast<uint8_t*>(&key)[depth];
      //TODO Fissioned loops slow if objects are not key-only (useless cache fill)?
		  ++bucket_size[key_cache[i]];
    }

		//Prefix sum
		std::array<data_type*, 256> bucket;
		bucket[0] = begin_cache;
      
		for(size_t i = 1; i < 256; ++i){
			bucket[i] = bucket[i - 1] + bucket_size[i - 1];	
		}	

		for(size_t i = 0; i < element_count; ++i){
			*(bucket[key_cache[i]]++) = std::move(*(begin_original + i));
		}	
    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
    //std::copy(data_cache.get(), data_cache.get() + element_count, begin);
  }

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
  // TODO CONTINUE
}

template <typename Iterator, typename KeyGetter>
static inline void radix_sort_par(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  TIME_START();
  static constexpr size_t core_count = 8;
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);


  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);
  TIME_PRINT_RESET("Setup time");

  //Start of actual work
  for (size_t depth = 0; depth < size_of_key; ++depth) {
    std::array<size_t, 256> bucket_size{0};  // Init to 0
    // Read bytes and count occurances

    //static schedule to minimise false sharing
    #pragma omp parallel for schedule(static) 
    for (size_t i = 0; i < element_count; ++i) {
      auto key = key_getter(*(begin_original + i)); 
      key_cache[i] = reinterpret_cast<uint8_t*>(&key)[depth];
    }
    TIME_PRINT_RESET("Create Cache");

    //TODO this is not running parallel?
    #pragma omp parallel
    {
      std::array<size_t, 256> private_bucket_size{0};  // Init to 0
      #pragma omp for schedule(static)
      for (size_t i = 0; i < element_count; ++i) {
        ++private_bucket_size[key_cache[i]];
      }
      #pragma omp critical
      for(size_t i = 0; i < private_bucket_size.size(); ++i){
        bucket_size[i] += private_bucket_size[i];
      } 
    }
    TIME_PRINT_RESET("Find bucket size");
    
		//Prefix sum
		std::array<data_type*, 256> bucket;
		bucket[0] = begin_cache;
    //same as std::array<data_type*, 256> bucket{begin_cache}?
      
		for(size_t i = 1; i < 256; ++i){
			bucket[i] = bucket[i - 1] + bucket_size[i - 1];	
		}	

    //TODO The greates performance hit comes here!  
    #pragma omp parallel num_threads(16)
    {
    std::array<data_type*, 256> bucket_local = bucket;
    const int thread_id = omp_get_thread_num();
    //#pragma omp for
		for(size_t i = 0; i < element_count; ++i){
      if(key_cache[i] % 16 == thread_id)
			*(bucket_local[key_cache[i]]++) = std::move(*(begin_original + i));
		}	
    }
    TIME_PRINT_RESET("Redistribute data");

    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
    //std::copy(data_cache.get(), data_cache.get() + element_count, begin);
  }

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
  // TODO CONTINUE
}

//typedef struct {unsigned value : 4;} nibble;

template <typename Iterator, typename KeyGetter>
static inline void radix_sort_par_nibble(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  TIME_START();
  static constexpr size_t core_count = 8;
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);


  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);
  TIME_PRINT_RESET("Setup time");

  //Start of actual work
  for (size_t depth = 0; depth < size_of_key * 2; ++depth) {
    std::array<size_t, 16> bucket_size{0};  // Init to 0
    // Read bytes and count occurances

    //static schedule to minimise false sharing
    const uint8_t shift = (depth & 1) * 4;
    const uint8_t mask = 0b00001111 << shift;
    #pragma omp parallel for schedule(static) 
    for (size_t i = 0; i < element_count; ++i) {
      auto key = key_getter(*(begin_original + i)); 
      key_cache[i] = reinterpret_cast<uint8_t*>(&key)[depth/2];
      key_cache[i] = (key_cache[i] & mask) >> shift;
    }
    TIME_PRINT_RESET("Create Cache");

    //TODO this is not running parallel?
    #pragma omp parallel
    {
      std::array<size_t, 16> private_bucket_size{0};  // Init to 0
      #pragma omp for schedule(static)
      for (size_t i = 0; i < element_count; ++i) {
        ++private_bucket_size[key_cache[i]];
      }
      #pragma omp critical
      for(size_t i = 0; i < private_bucket_size.size(); ++i){
        bucket_size[i] += private_bucket_size[i];
      } 
    }
    TIME_PRINT_RESET("Find bucket size");
    
		//Prefix sum
		std::array<data_type*, 16> bucket;
		bucket[0] = begin_cache;
    //same as std::array<data_type*, 256> bucket{begin_cache}?
      
		for(size_t i = 1; i < 16; ++i){
			bucket[i] = bucket[i - 1] + bucket_size[i - 1];	
		}	

    //TODO The greates performance hit comes here!  
    {
    //#pragma omp for
		for(size_t i = 0; i < element_count; ++i){
			*(bucket[key_cache[i]]++) = std::move(*(begin_original + i));
		}	
    }
    TIME_PRINT_RESET("Redistribute data");

    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
    //std::copy(data_cache.get(), data_cache.get() + element_count, begin);
  }

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
  // TODO CONTINUE
}


}  // namespace rdx
