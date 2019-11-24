#pragma once
#include <chrono>


#define TIME_FUNCTION(function, comment) \
  {\
  auto t1 = std::chrono::high_resolution_clock::now();\
  function;\
  auto t2 = std::chrono::high_resolution_clock::now();\
  auto int_ms = std::chrono::duration_cast<std::chrono::milliseconds>(t2 - t1);\
  std::cout << "[DEBUG TIMER] " << comment << int_ms.count()/1000.0f << "s" << std::endl;\
  }
