if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-O3 -ggdb -Wall -Wall")

find_package(OpenMP REQUIRED)

#Create lib
add_library(radix_sort STATIC)
target_sources(radix_sort
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/radix_sort_prefix.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/debug_helper.hpp"
  )

set_target_properties(radix_sort PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(radix_sort PUBLIC OpenMP::OpenMP_CXX)
target_include_directories(radix_sort PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}")
#target_link_libraries(radix_sort PUBLIC enet PRIVATE common)
