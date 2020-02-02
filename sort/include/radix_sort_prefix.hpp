#pragma once
#include <algorithm>
#include <array>
#include <vector>
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
template <typename Iterator, typename KeyGetter>
static inline void radix_sort_prefix_par(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  TIME_START();

  //Setup
  const size_t thread_count = omp_get_max_threads();
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  //The key cache contains the key value for the current radix
  //iteration
  std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  //Data cache, a buffer which will be used to write the result of a
  //radix step into. Notice, impl. is out of place.
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);

  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);

  TIME_PRINT_RESET("Setup time");

  //2D array holding the bucket sizes for each thread
  std::vector<std::array<size_t, 256>> bucket_sizes(thread_count, {0});

  //Start of actual work//////////////////
  for (size_t depth = 0; depth < size_of_key; ++depth) {
    //create the key cache
    //static schedule to minimise false sharing
    #pragma omp parallel for schedule(static) 
    for (size_t i = 0; i < element_count; ++i) {
      auto key = key_getter(*(begin_original + i)); 
      key_cache[i] = reinterpret_cast<uint8_t*>(&key)[depth];
    }
    TIME_PRINT_RESET("Create Cache");

    //eval. the bucket sizes for each thread
    #pragma omp parallel
    {
      std::array<size_t, 256> private_bucket_size{0};  // Init to 0
      #pragma omp for schedule(static)
      for (size_t i = 0; i < element_count; ++i) {
        ++private_bucket_size[key_cache[i]];
      }

      bucket_sizes[omp_get_thread_num()] = std::move(private_bucket_size);
    }
    TIME_PRINT_RESET("Find bucket size");
    
    //Snake prefix sum
    std::vector<std::array<data_type*, 256>> buckets(thread_count, {0});
    buckets[0][0] = begin_cache;
    for(size_t index = 0; index < 256; ++index){
      for(size_t thread = 1; thread < thread_count; ++thread){
        buckets[thread][index] = buckets[thread - 1][index] + bucket_sizes[thread - 1][index];
        //std::cout << "i:" << index << " t:" << thread << " at: " <<(uint64_t) (buckets[thread][index] - begin_cache) << '\n';
      }
      if(index < 255){
        buckets[0][index + 1] = buckets[thread_count - 1][index] + bucket_sizes[thread_count - 1][index];
      }
    }
    TIME_PRINT_RESET("Create initial buckets");

    //Redistribute the data
    #pragma omp parallel
    {
      //Make local copy, to minimise false sharing at the boundaries 
      //note, std::move would not prevent?
      std::array<data_type*, 256> bucket_local = buckets[omp_get_thread_num()];
      #pragma omp for schedule(static)
      for(size_t i = 0; i < element_count; ++i){
        *(bucket_local[key_cache[i]]++) = std::move(*(begin_original + i));
      }	
    }
    TIME_PRINT_RESET("Redistribute data");

    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
  }
  //End of actual work//////////////////////

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
}

template <typename Iterator, typename KeyGetter>
static inline void radix_sort_prefix_par_nibble(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  TIME_START();

  //Setup
  const size_t thread_count = omp_get_max_threads();
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  //The key cache contains the key value for the current radix
  //iteration
  std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  //Data cache, a buffer which will be used to write the result of a
  //radix step into. Notice, impl. is out of place.
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);

  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);

  TIME_PRINT_RESET("Setup time");

  //2D array holding the bucket sizes for each thread
  std::vector<std::array<size_t, 16>> bucket_sizes(thread_count, {0});

  //Start of actual work//////////////////
  for (size_t depth = 0; depth < size_of_key * 2; ++depth) {
    if(depth & 0x1){
      //create the key cache
      //static schedule to minimise false sharing
      #pragma omp parallel for schedule(static) 
      for (size_t i = 0; i < element_count; ++i) {
        auto key = key_getter(*(begin_original + i)); 
        key_cache[i] = reinterpret_cast<uint8_t*>(&key)[depth/2] >> 4;
      }
    } else{
      #pragma omp parallel for schedule(static) 
      for (size_t i = 0; i < element_count; ++i) {
        auto key = key_getter(*(begin_original + i)); 
        key_cache[i] = reinterpret_cast<uint8_t*>(&key)[depth/2] & 0x0f;
      }
    }
    TIME_PRINT_RESET("Create Cache");

    //eval. the bucket sizes for each thread
    #pragma omp parallel
    {
      std::array<size_t, 16> private_bucket_size{0};  // Init to 0
      #pragma omp for schedule(static)
      for (size_t i = 0; i < element_count; ++i) {
        ++private_bucket_size[key_cache[i]];
      }

      bucket_sizes[omp_get_thread_num()] = std::move(private_bucket_size);
    }
    TIME_PRINT_RESET("Find bucket size");
    
    //Snake prefix sum
    std::vector<std::array<data_type*, 16>> buckets(thread_count, {0});
    buckets[0][0] = begin_cache;
    for(size_t index = 0; index < 16; ++index){
      for(size_t thread = 1; thread < thread_count; ++thread){
        buckets[thread][index] = buckets[thread - 1][index] + bucket_sizes[thread - 1][index];
        //std::cout << "i:" << index << " t:" << thread << " at: " <<(uint64_t) (buckets[thread][index] - begin_cache) << '\n';
      }
      if(index < 15){
        buckets[0][index + 1] = buckets[thread_count - 1][index] + bucket_sizes[thread_count - 1][index];
      }
    }
    TIME_PRINT_RESET("Create initial buckets");

    //Redistribute the data
    #pragma omp parallel
    {
      //Make local copy, to minimise false sharing at the boundaries 
      //note, std::move would not prevent?
      std::array<data_type*, 16> bucket_local = buckets[omp_get_thread_num()];
      #pragma omp for schedule(static)
      for(size_t i = 0; i < element_count; ++i){
        *(bucket_local[key_cache[i]]++) = std::move(*(begin_original + i));
      }	
    }
    TIME_PRINT_RESET("Redistribute data");

    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
  }
  //End of actual work//////////////////////

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
}

template <typename Iterator, typename KeyGetter>
static inline void radix_sort_prefix_par_short(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  TIME_START();

  //Setup
  const size_t thread_count = omp_get_max_threads();
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  //The key cache contains the key value for the current radix
  //iteration
  std::unique_ptr<uint16_t[]> key_cache(new uint16_t[element_count]);
  //Data cache, a buffer which will be used to write the result of a
  //radix step into. Notice, impl. is out of place.
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);

  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);

  TIME_PRINT_RESET("Setup time");

  //2D array holding the bucket sizes for each thread
  std::vector<std::array<size_t, 65536>> bucket_sizes(thread_count, {0});

  //Start of actual work//////////////////
  for (size_t depth = 0; depth < size_of_key/2; ++depth) {
    //create the key cache
    //static schedule to minimise false sharing
    #pragma omp parallel for schedule(static) 
    for (size_t i = 0; i < element_count; ++i) {
      auto key = key_getter(*(begin_original + i)); 
      key_cache[i] = reinterpret_cast<uint16_t*>(&key)[depth];
    }
    TIME_PRINT_RESET("Create Cache");

    //eval. the bucket sizes for each thread
    #pragma omp parallel
    {
      std::array<size_t, 65536> private_bucket_size{0};  // Init to 0
      #pragma omp for schedule(static)
      for (size_t i = 0; i < element_count; ++i) {
        ++private_bucket_size[key_cache[i]];
      }

      bucket_sizes[omp_get_thread_num()] = std::move(private_bucket_size);
    }
    TIME_PRINT_RESET("Find bucket size");
    
    //Snake prefix sum
    std::vector<std::array<data_type*, 65536>> buckets(thread_count, {0});
    buckets[0][0] = begin_cache;
    for(size_t index = 0; index < 65536; ++index){
      for(size_t thread = 1; thread < thread_count; ++thread){
        buckets[thread][index] = buckets[thread - 1][index] + bucket_sizes[thread - 1][index];
        //std::cout << "i:" << index << " t:" << thread << " at: " <<(uint64_t) (buckets[thread][index] - begin_cache) << '\n';
      }
      if(index < 65535){
        buckets[0][index + 1] = buckets[thread_count - 1][index] + bucket_sizes[thread_count - 1][index];
      }
    }
    TIME_PRINT_RESET("Create initial buckets");

    //Redistribute the data
    #pragma omp parallel
    {
      //Make local copy, to minimise false sharing at the boundaries 
      //note, std::move would not prevent?
      std::array<data_type*, 65536> bucket_local = buckets[omp_get_thread_num()];
      #pragma omp for schedule(static)
      for(size_t i = 0; i < element_count; ++i){
        *(bucket_local[key_cache[i]]++) = std::move(*(begin_original + i));
      }	
    }
    TIME_PRINT_RESET("Redistribute data");

    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
  }
  //End of actual work//////////////////////

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
}

template <typename Iterator, typename KeyGetter>
static inline void radix_sort_prefix_par_no_cache(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  TIME_START();

  //Setup
  const size_t thread_count = omp_get_max_threads();
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  //The key cache contains the key value for the current radix
  //iteration
  //std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  //Data cache, a buffer which will be used to write the result of a
  //radix step into. Notice, impl. is out of place.
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);

  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);

  TIME_PRINT_RESET("Setup time");

  //2D array holding the bucket sizes for each thread
  std::vector<std::array<size_t, 256>> bucket_sizes(thread_count, {0});

  //Start of actual work//////////////////
  for (size_t depth = 0; depth < size_of_key; ++depth) {

    auto get_depth_key = [=](size_t i){
      const auto key = key_getter(begin_original[i]);
      //key = key_getter(begin_original[i]);
      return reinterpret_cast<const uint8_t*>(&key)[depth];
    };

    TIME_PRINT_RESET("Create Cache");

    //eval. the bucket sizes for each thread
    #pragma omp parallel
    {
      std::array<size_t, 256> private_bucket_size{0};  // Init to 0
      #pragma omp for schedule(static)
      for (size_t i = 0; i < element_count; ++i) {
        ++private_bucket_size[get_depth_key(i)];
      }

      bucket_sizes[omp_get_thread_num()] = std::move(private_bucket_size);
    }
    TIME_PRINT_RESET("Find bucket size");
    
    //Snake prefix sum
    std::vector<std::array<data_type*, 256>> buckets(thread_count, {0});
    buckets[0][0] = begin_cache;
    for(size_t index = 0; index < 256; ++index){
      for(size_t thread = 1; thread < thread_count; ++thread){
        buckets[thread][index] = buckets[thread - 1][index] + bucket_sizes[thread - 1][index];
        //std::cout << "i:" << index << " t:" << thread << " at: " <<(uint64_t) (buckets[thread][index] - begin_cache) << '\n';
      }
      if(index < 255){
        buckets[0][index + 1] = buckets[thread_count - 1][index] + bucket_sizes[thread_count - 1][index];
      }
    }
    TIME_PRINT_RESET("Create initial buckets");

    //Redistribute the data
    #pragma omp parallel
    {
      //Make local copy, to minimise false sharing at the boundaries 
      //note, std::move would not prevent?
      std::array<data_type*, 256> bucket_local = buckets[omp_get_thread_num()];
      #pragma omp for schedule(static)
      for(size_t i = 0; i < element_count; ++i){
        *(bucket_local[get_depth_key(i)]++) = std::move(*(begin_original + i));
      }	
    }
    TIME_PRINT_RESET("Redistribute data");

    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
  }
  //End of actual work//////////////////////

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
}

template <int cache_size, typename Iterator, typename KeyGetter>
static inline void radix_sort_prefix_par_no_cache_write_back_cache(const Iterator begin, const Iterator end,
                                  const KeyGetter key_getter) {
  TIME_START();

  //Setup
  const size_t thread_count = omp_get_max_threads();
  typedef typename std::iterator_traits<Iterator>::value_type data_type;
  constexpr const size_t size_of_key = sizeof(key_getter(*begin));
  const size_t element_count = std::distance(begin, end);

  //The key cache contains the key value for the current radix
  //iteration
  //std::unique_ptr<uint8_t[]> key_cache(new uint8_t[element_count]);
  //Data cache, a buffer which will be used to write the result of a
  //radix step into. Notice, impl. is out of place.
  std::unique_ptr<data_type[]> data_cache(new data_type[element_count]);

  //We use pointers internally; we don't have concepts yet...
  data_type* begin_original = &*begin;
  data_type* end_original = &*end;
  data_type* begin_cache = data_cache.get();
  data_type* end_cache = &(data_cache[element_count - 1]);

  TIME_PRINT_RESET("Setup time");

  //2D array holding the bucket sizes for each thread
  std::vector<std::array<size_t, 256>> bucket_sizes(thread_count, {0});

  //Start of actual work//////////////////
  for (size_t depth = 0; depth < size_of_key; ++depth) {

    auto get_depth_key = [=](size_t i){
      const auto key = key_getter(begin_original[i]);
      //key = key_getter(begin_original[i]);
      return reinterpret_cast<const uint8_t*>(&key)[depth];
    };

    TIME_PRINT_RESET("Create Cache");

    //eval. the bucket sizes for each thread
    #pragma omp parallel
    {
      std::array<size_t, 256> private_bucket_size{0};  // Init to 0
      #pragma omp for schedule(static)
      for (size_t i = 0; i < element_count; ++i) {
        ++private_bucket_size[get_depth_key(i)];
      }

      bucket_sizes[omp_get_thread_num()] = std::move(private_bucket_size);
    }
    TIME_PRINT_RESET("Find bucket size");
    
    //Snake prefix sum
    std::vector<std::array<data_type*, 256>> buckets(thread_count, {0});
    buckets[0][0] = begin_cache;
    for(size_t index = 0; index < 256; ++index){
      for(size_t thread = 1; thread < thread_count; ++thread){
        buckets[thread][index] = buckets[thread - 1][index] + bucket_sizes[thread - 1][index];
        //std::cout << "i:" << index << " t:" << thread << " at: " <<(uint64_t) (buckets[thread][index] - begin_cache) << '\n';
      }
      if(index < 255){
        buckets[0][index + 1] = buckets[thread_count - 1][index] + bucket_sizes[thread_count - 1][index];
      }
    }
    TIME_PRINT_RESET("Create initial buckets");

    //Redistribute the data
    #pragma omp parallel
    {
      //Make local copy, to minimise false sharing at the boundaries 
      //note, std::move would not prevent?
      std::array<data_type*, 256> bucket_local = buckets[omp_get_thread_num()];

      constexpr int csize = cache_size;
      std::array<std::array<data_type, csize>,256> local_cache;
      std::array<uint8_t, 256> local_cache_size{0};
      

      #pragma omp for schedule(static)
      for(size_t i = 0; i < element_count; ++i){
        const auto k = get_depth_key(i);
        local_cache[k][local_cache_size[k]] = std::move(*(begin_original + i));
        local_cache_size[k]++;
        if(local_cache_size[k] == csize){
          std::move(local_cache[k].begin(),local_cache[k].begin() + csize, bucket_local[k]);
          bucket_local[k] += csize;
          local_cache_size[k] = 0;
        }
      }	
      for(int i = 0; i < 256; ++i){
        std::move(local_cache[i].begin(), local_cache[i].begin() + local_cache_size[i], bucket_local[i]);
      }
    }
    TIME_PRINT_RESET("Redistribute data");

    //We could actually be much faster with a swap (two moves), but I need the
    //whole object not just iterators.
    std::swap(begin_original, begin_cache);
    std::swap(end_original, end_cache);
  }
  //End of actual work//////////////////////

  //If number of iterations was odd (we need to copy)
  if(size_of_key & 1){
    std::move(data_cache.get(), data_cache.get() + element_count, begin);
  }
}

}  // namespace rdx
