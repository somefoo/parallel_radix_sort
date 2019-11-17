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


