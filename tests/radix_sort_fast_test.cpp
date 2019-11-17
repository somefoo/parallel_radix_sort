#include <radix_sort.hpp>
#include <array>

template <typename Iterator, typename Comparator>
int is_sorted(Iterator begin, Iterator end, Comparator comp){
  for(Iterator start = begin; start < end; ++start){
    if(!comp(*begin, *end)) return 1;
  }
  return 0;
}

int main(){
  std::array elements{43,515,645,3,12,5,64523,5423,43,6454,43};
  auto comp = [](const int& a, const int& b){ return a<b;};

  rdx::radix_sort(elements.begin(), elements.end(), comp, 32);

  return is_sorted(elements.begin(), elements.end(), comp);
}

