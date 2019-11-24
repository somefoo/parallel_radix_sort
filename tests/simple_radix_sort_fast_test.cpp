#include <array>
#include <vector>
#include <simple_radix_sort.hpp>
int main() {
  std::vector<uint32_t> values(10000000);
  std::generate(values.begin(), values.end(), std::rand);

  rdx::simple_radix_sort_seq(&(*values.begin()), &(*values.end()));
//  rdx::radix_sort(elements_pair.begin(), elements_pair.end(), comp_pair);

  bool sorted = std::is_sorted(values.begin(), values.end());
  if(sorted){
    std::cout << "[SUCCESS] Sorting small struct (key,value) array.\n"; 
  }else{
    std::cout << "[FAILED] Sorting small integer array.\n"; 
    return 1;
  }

  return 0;
}
