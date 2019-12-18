if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()


set (CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -Wall -Wextra")
set(CMAKE_CXX_FLAGS_DEBUG "-Og -Wall -Wextra")

find_package(OpenMP REQUIRED)

#Create lib
add_library(radix_sort STATIC)
target_sources(radix_sort
  PRIVATE 
    "src/radix_sort.cpp"
    "src/simple_radix_sort.cpp"
  PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/include/radix_sort.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/simple_radix_sort.hpp"
    "${CMAKE_CURRENT_SOURCE_DIR}/include/debug_helper.hpp"
  )
set_target_properties(radix_sort PROPERTIES LINKER_LANGUAGE CXX)
target_link_libraries(radix_sort PUBLIC OpenMP::OpenMP_CXX)
target_include_directories(radix_sort PUBLIC "${CMAKE_CURRENT_SOURCE_DIR}/include")
#target_link_libraries(radix_sort PUBLIC enet PRIVATE common)
