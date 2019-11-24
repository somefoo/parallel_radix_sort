if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

message(STATUS "Building tests...")
set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra -Werror -Wno-unused-private-field")
set(CMAKE_CXX_FLAGS_DEBUG "-g -Wall -Wextra")

add_executable(radix_sort_fast_test radix_sort_fast_test.cpp)
target_link_libraries(radix_sort_fast_test PRIVATE radix_sort)
add_test(RadixSortFastTest radix_sort_fast_test)

add_executable(radix_sort_seq_fast_test radix_sort_seq_fast_test.cpp)
target_link_libraries(radix_sort_seq_fast_test PRIVATE radix_sort)
add_test(RadixSortSeqFastTest radix_sort_seq_fast_test)


add_executable(simple_radix_sort_seq_fast_test simple_radix_sort_fast_test.cpp)
target_link_libraries(simple_radix_sort_seq_fast_test PRIVATE radix_sort)
add_test(SimpleRadixSortSeqFastTest simple_radix_sort_seq_fast_test)

