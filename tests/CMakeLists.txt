if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Building tests...")
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O3 -ggdb -Wall -Wextra")

add_executable(radix_sort_fast_test radix_sort_fast_test.cpp control.hpp)
target_link_libraries(radix_sort_fast_test PRIVATE radix_sort)
add_test(RadixSortFastTest radix_sort_fast_test)

add_executable(radix_sort_par_fast_test radix_sort_par_fast_test.cpp control.hpp)
target_link_libraries(radix_sort_par_fast_test PRIVATE radix_sort)
add_test(RadixSortParFastTest radix_sort_par_fast_test)

add_executable(radix_sort_seq_test radix_sort_seq_test.cpp control.hpp)
target_link_libraries(radix_sort_seq_test PRIVATE radix_sort)
add_test(RadixSortSeqTest radix_sort_seq_test)

add_executable(radix_sort_par_test radix_sort_par_test.cpp control.hpp)
target_link_libraries(radix_sort_par_test PRIVATE radix_sort)
add_test(RadixSortParTest radix_sort_par_test)

add_executable(radix_sort_par_nibble_test radix_sort_par_nibble_test.cpp control.hpp)
target_link_libraries(radix_sort_par_nibble_test PRIVATE radix_sort)
add_test(RadixSortParNibbleTest radix_sort_par_nibble_test)

add_executable(simple_radix_sort_seq_fast_test simple_radix_sort_fast_test.cpp control.hpp)
target_link_libraries(simple_radix_sort_seq_fast_test PRIVATE radix_sort)

add_executable(radix_sort_compare radix_sort_compare.cpp control.hpp)
target_link_libraries(radix_sort_compare PRIVATE radix_sort)
add_test(SimpleRadixSortSeqFastTest simple_radix_sort_seq_fast_test)

add_executable(radix_sort_par_short_test radix_sort_par_short_test.cpp control.hpp)
target_link_libraries(radix_sort_par_short_test PRIVATE radix_sort)
add_test(RadixSortParShortTest radix_sort_par_short_test)
