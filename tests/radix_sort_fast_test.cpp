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

//template <typename Iterator, typename Comparator>
//bool is_sorted(const Iterator begin, const Iterator end, const Comparator comp) {
//  for (Iterator current = begin; current < end - 1; ++current) {
//   if (!comp(*current, *(current + 1))) return false;
//  }
//  return true;
//}

int main() {
  std::array<uint32_t,10> elements{43, 515, 645, 3, 12, 5, 64523, 5423, 43, 6454};
  std::vector<key_data_pair> elements_pair(1000000);

  auto comp = [](const uint32_t a, const uint32_t b) { return a < b; };
  auto comp_pair = [](const key_data_pair& a, const key_data_pair& b) {return a.key < b.key;};

  rdx::radix_sort(elements.begin(), elements.end(), comp);
  rdx::radix_sort(elements_pair.begin(), elements_pair.end(), comp_pair);

  bool sorted = std::is_sorted(elements.begin(), elements.end(), comp);
  if(sorted){
    std::cout << "[SUCCESS] Sorting small integer array.\n"; 
  }else{
    std::cout << "[FAILED] Sorting small integer array.\n"; 
    return 1;
  }
  sorted = std::is_sorted(elements_pair.begin(), elements_pair.end(), comp_pair);
  if(sorted){
    std::cout << "[SUCCESS] Sorting small struct (key,value) array.\n"; 
  }else{
    std::cout << "[FAILED] Sorting small integer array.\n"; 
    return 1;
  }

  return 0;
}
