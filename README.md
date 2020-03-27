# Parallel Radix Sort
This is a header only implementation of a parallel radix sort using OpenMP. For more information, look at the header files. 

Example of use:

```cpp
#include "radix_sort_prefix.hpp"
#include <algorithm>
#include <vector>
#include <stdint.h>

int main() {
  //Create array with random values
  std::vector<uint32_t> values(1000000);
  std::generate(values.begin(), values.end(), std::rand);

  //Getter function for key of data to be sorted and sort
  auto getter = [](const uint32_t& val) { return val; };
  rdx::radix_sort_prefix_par(values.begin(), values.end(), getter);
  
  //rdx::radix_sort_prefix_par_no_cache(values.begin(), values.end(), getter);
  //rdx::radix_sort_prefix_par_no_cache_write_back_cache(values.begin(), values.end(), getter);
  
  return 0;
}
```
