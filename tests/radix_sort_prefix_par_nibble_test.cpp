#include <array>
#include <vector>
#include "control.hpp"
#include <radix_sort_prefix.hpp>
#include <radix_sort.hpp>
#include <iomanip>
#if 0
struct key_data_pair {
  key_data_pair() {
    static uint32_t up_counter = 0;
    key = up_counter % 59159;
    up_counter += 163417;
  }
  uint32_t key;
  uint32_t data[11]{0xffffff};
};

int main() {
  std::vector<key_data_pair> elements_pair(100);
  std::vector<key_data_pair> elements_pair_old(elements_pair);
  std::vector<key_data_pair> elements_pair_sorted(elements_pair);

  auto comp_pair = [](const key_data_pair& a, const key_data_pair& b) {return a.key < b.key;};
  auto key_getter_pair = [](const key_data_pair& a) {return a.key;};
  

  rdx::radix_sort_prefix_par(elements_pair.begin(), elements_pair.end(), key_getter_pair);
  rdx::radix_sort_seq(elements_pair_sorted.begin(), elements_pair_sorted.end(), key_getter_pair);

  bool sorted = std::is_sorted(elements_pair.begin(), elements_pair.end(), comp_pair);
  if(sorted){
    std::cout << "[SUCCESS] Sorting small struct (key,value) array.\n"; 
  }else{
    std::cout << "[FAILED] Sorting small integer array.\n"; 
    for(size_t i = 0; i < elements_pair.size(); ++i){
     std::cout << "o:n:s " << std::setfill('0') << std::setw(4) << std::hex << elements_pair_old[i].key << ":" << std::setfill('0') << std::setw(4) << std::hex << elements_pair[i].key << ":" << std::setfill('0') << std::setw(4) << elements_pair_sorted[i].key <<'\n';
    }
    return 1;
  }

  return 0;
}
#else
int main() {
  std::vector<uint32_t> values(value_count);
  std::generate(values.begin(), values.end(), std::rand);

	auto getter = [](const uint32_t& val){return val;};
  rdx::radix_sort_prefix_par_nibble(values.begin(), values.end(), getter);
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

#endif
