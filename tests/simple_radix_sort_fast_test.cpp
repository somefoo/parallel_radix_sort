#include <array>
#include <vector>
#include <simple_radix_sort.hpp>
int main() {
  std::array<uint32_t, 10> values = {10,9,7,8,6,5,4,2,1,3};
  std::array<uint32_t, 10> values_old = values;

  rdx::simple_radix_sort_seq(values.begin(), values.end());
//  rdx::radix_sort(elements_pair.begin(), elements_pair.end(), comp_pair);

  bool sorted = std::is_sorted(values.begin(), values.end());
  if(sorted){
    std::cout << "[SUCCESS] Sorting small struct (key,value) array.\n"; 
  }else{
    std::cout << "[FAILED] Sorting small integer array.\n"; 
    for(size_t i = 0; i < values.size(); ++i){
     std::cout << "o:n " << values_old[i] << ":" << values[i] << '\n';
    }
    return 1;
  }

  return 0;
}
