#include <array>
#include <vector>
#include "control.hpp"
#include <radix_sort.hpp>
#include <radix_sort_prefix.hpp>
#include <iomanip>
#include <debug_helper.hpp>
#include <tlx/sort/parallel_mergesort.hpp>
#include <tlx/sort/parallel_radixsort.hpp>


int main() {
	constexpr size_t min_count = 1024000l;
	constexpr size_t max_count = 1024000l * 4096l;
	constexpr size_t repeat = 5;

  std::vector<uint64_t> values(min_count);
  std::generate(values.begin(), values.end(), std::rand);
  std::vector<uint64_t> values2(min_count);
  values2 = values;

	auto getter = [](const auto& val){return val;};

	auto time_average = [&](auto f, size_t count, std::string name){
		auto total_time = 0;
		for(size_t i = 0; i < repeat; ++i){
			auto t1 = std::chrono::high_resolution_clock::now();
			f();
			auto t2 = std::chrono::high_resolution_clock::now();\
			total_time += std::chrono::duration_cast<std::chrono::microseconds>(t2 - t1).count();\
			values2 = values;
		}
		total_time = total_time / repeat;
		std::cout << "algo=" << name << "\tcount=" << count << "\trepeat=" << repeat << "\tavg_time=" << std::fixed << std::setprecision(10) << (double) total_time / (double) 1e+6 << std::endl;
	};

  auto run_tlx_mergesort = [&](){
    tlx::parallel_mergesort(values2.begin(), values2.end());
  };

  auto run_tlx_radixsort_8 = [&]() {
    using Iterator = typename std::vector<uint64_t>::iterator;
    using T = typename std::iterator_traits<Iterator>::value_type;
    using key_type = uint8_t;

    tlx::parallel_radixsort_detail::radix_sort<
        Iterator, tlx::parallel_radixsort_detail::get_key<T, key_type>>(
                values2.begin(), values2.end(), sizeof(T)/sizeof(key_type));
   };

  auto run_prefix_par_no_cache_write_back_cache = [&](){
    rdx::radix_sort_prefix_par_no_cache_write_back_cache(values2.begin(), values2.end(), getter); 
  };

  auto run_prefix_par_no_cache = [&](){
    rdx::radix_sort_prefix_par_no_cache(values2.begin(), values2.end(), getter); 
  };

  auto run_prefix_par = [&](){
    rdx::radix_sort_prefix_par(values2.begin(), values2.end(), getter); 
  };

  auto run_prefix_par_short = [&](){
    rdx::radix_sort_prefix_par_short(values2.begin(), values2.end(), getter); 
  };

  auto run_prefix_par_nibble = [&](){
    rdx::radix_sort_prefix_par_nibble(values2.begin(), values2.end(), getter); 
  };

  auto run_par = [&](){
    rdx::radix_sort_par(values2.begin(), values2.end(), getter); 
  };

  auto run_par_short = [&](){
    rdx::radix_sort_par_short(values2.begin(), values2.end(), getter); 
  };

  auto run_par_nibble = [&](){
    rdx::radix_sort_par_nibble(values2.begin(), values2.end(), getter); 
  };

  auto run_seq = [&](){
    rdx::radix_sort_seq(values2.begin(), values2.end(), getter); 
  };

  auto run_std_sort = [&](){
    std::sort(values2.begin(), values2.end()); 
  };

  //std::cout << "\talgo" << "\tcount" << "\trepeat" << "\tavg_time";
	for(size_t count = min_count; count <= max_count; count *= 2){
    values = std::vector<uint64_t>(count); 
    std::generate(values.begin(), values.end(), std::rand);
    values2 = values;
		time_average(run_tlx_radixsort_8, count,                        "tlx_msd_sort                        ");
		time_average(run_tlx_mergesort, count,                          "tlx_merge_sort                      ");
		time_average(run_prefix_par_no_cache_write_back_cache, count,   "prefix_par_no_cache_write_back_cache");
		time_average(run_prefix_par_no_cache, count,                    "prefix_par_no_cache                 ");
		time_average(run_prefix_par, count,                           "prefix_par_(byte)                   ");
		time_average(run_prefix_par_short, count,                     "prefix_par_(short)                  ");
		time_average(run_prefix_par_nibble, count,                    "prefix_par_(nibble)                 ");
		time_average(run_par, count,                                  "par_(byte)                          ");
		time_average(run_par_nibble, count,                           "par_(nibble)                        ");
		time_average(run_par_short, count,                            "par_(short)                         ");
		time_average(run_seq, count,                                  "seq_(byte)                          ");
		time_average(run_std_sort, count,                             "std_sort                            ");
	}


  return 0;
}

