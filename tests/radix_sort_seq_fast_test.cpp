#include <array>
#include <vector>
#include <radix_sort.hpp>
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
  std::vector<key_data_pair> elements_pair(1000);
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
     std::cout << "o:n " << elements_pair_old[i].key << ":" << elements_pair[i].key << '\n';
    }
    return 1;
  }

  return 0;
}
