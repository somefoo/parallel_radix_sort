#include <array>
#include <vector>
#include "control.hpp"
#include <radix_sort.hpp>
#include <radix_sort_prefix.hpp>
#include <iomanip>
#include <debug_helper.hpp>


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
	
  auto run_prefix_par_no_cache = [&](){
    rdx::radix_sort_prefix_par_no_cache(values2.begin(), values2.end(), getter); 
  };

  auto run_prefix_par_no_cache_write_back_cache_64 = [&](){
    rdx::radix_sort_prefix_par_no_cache_write_back_cache<64>(values2.begin(), values2.end(), getter); 
  };


  auto run_prefix_par_no_cache_write_back_cache_128 = [&](){
    rdx::radix_sort_prefix_par_no_cache_write_back_cache<128>(values2.begin(), values2.end(), getter); 
  };


  auto run_prefix_par_no_cache_write_back_cache_256 = [&](){
    rdx::radix_sort_prefix_par_no_cache_write_back_cache<256>(values2.begin(), values2.end(), getter); 
  };


  auto run_prefix_par_no_cache_write_back_cache_512 = [&](){
    rdx::radix_sort_prefix_par_no_cache_write_back_cache<512>(values2.begin(), values2.end(), getter); 
  };


  auto run_prefix_par_no_cache_write_back_cache_1024 = [&](){
    rdx::radix_sort_prefix_par_no_cache_write_back_cache<1024>(values2.begin(), values2.end(), getter); 
  };


  //std::cout << "\talgo" << "\tcount" << "\trepeat" << "\tavg_time";
	for(size_t count = min_count; count <= max_count; count *= 2){
    values = std::vector<uint64_t>(count); 
    std::generate(values.begin(), values.end(), std::rand);
    values2 = values;
		time_average(run_prefix_par_no_cache, count,                       "prefix_par_no_cache                      ");
		time_average(run_prefix_par_no_cache_write_back_cache_64, count,   "prefix_par_no_cache_write_back_cache_64  ");
		time_average(run_prefix_par_no_cache_write_back_cache_128, count,  "prefix_par_no_cache_write_back_cache_128 ");
		time_average(run_prefix_par_no_cache_write_back_cache_256, count,  "prefix_par_no_cache_write_back_cache_256 ");
		time_average(run_prefix_par_no_cache_write_back_cache_512, count,  "prefix_par_no_cache_write_back_cache_512 ");
		time_average(run_prefix_par_no_cache_write_back_cache_1024, count, "prefix_par_no_cache_write_back_cache_1028");
	}


  return 0;
}
