#include <array>
#include <vector>
#include "control.hpp"
#include <radix_sort_prefix.hpp>
#include <iomanip>
#include <debug_helper.hpp>
#if 0
struct key_data_pair {
  key_data_pair() {
    static uint32_t up_counter = 0;
    key = up_counter % 59159;
    up_counter += 163417;
  }
  uint32_t key;
  uint32_t data[11];
};

int main() {
  std::vector<key_data_pair> elements_pair(100);
  std::vector<key_data_pair> elements_pair_old(elements_pair);

  auto comp_pair = [](const key_data_pair& a, const key_data_pair& b) {return a.key < b.key;};
  auto key_getter_pair = [](const key_data_pair& a) {return a.key;};
  

  rdx::radix_sort_seq(elements_pair.begin(), elements_pair.end(), key_getter_pair);
//  rdx::radix_sort(elements_pair.begin(), elements_pair.end(), comp_pair);

  bool sorted = std::is_sorted(elements_pair.begin(), elements_pair.end(), comp_pair);
  if(sorted){
    std::cout << "[SUCCESS] Sorting small struct (key,value) array.\n"; 
  }else{
    std::cout << "[FAILED] Sorting small integer array.\n"; 
    for(size_t i = 0; i < elements_pair.size(); ++i){
     std::cout << "o:n " << std::setfill('0') << std::setw(4) << std::hex << elements_pair_old[i].key << ":" << std::setfill('0') << std::setw(4) << std::hex << elements_pair[i].key << '\n';
    }
    return 1;
  }

  return 0;
}
#else
int main() {
  std::vector<uint32_t> values(value_count);
  std::generate(values.begin(), values.end(), std::rand);
  std::vector<uint32_t> values2(value_count);
  values2 = values;

  bool sorted = 1;
	auto getter = [](const auto& val){return val;};

  TIME_FUNCTION(rdx::radix_sort_prefix_par(values2.begin(), values2.end(), getter);, " par. prefix byte time");
  sorted &= std::is_sorted(values2.begin(), values2.end());
  values2 = values;

  TIME_FUNCTION(rdx::radix_sort_prefix_par_no_cache(values2.begin(), values2.end(), getter);, " par. prefix no cache time");
  sorted &= std::is_sorted(values2.begin(), values2.end());
  values2 = values;

  TIME_FUNCTION(rdx::radix_sort_prefix_par_no_cache_write_back_cache(values2.begin(), values2.end(), getter);, " par. prefix no cache write back cache time");
  sorted &= std::is_sorted(values2.begin(), values2.end());
  values2 = values;

//  rdx::radix_sort(elements_pair.begin(), elements_pair.end(), comp_pair);

  if(sorted){
    std::cout << "[SUCCESS] Sorting small struct (key,value) array.\n";
  }else{
    std::cout << "[FAILED] Sorting small integer array.\n";
    return 1;
  }

  return 0;
}

#endif
