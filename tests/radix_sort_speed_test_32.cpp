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

  std::vector<uint32_t> values(min_count);
  std::generate(values.begin(), values.end(), std::rand);
  std::vector<uint32_t> values2(min_count);
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
		std::cout << "algo=" << name << "\tcount=" << count << "\trepeat=" << repeat << "\tavg_time=" << std::fixed << std::setprecision(10) << (double) total_time / (double) 1e+6 << std::endl;
		total_time = total_time / repeat;
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

  //std::cout << "\talgo" << "\tcount" << "\trepeat" << "\tavg_time";
	for(size_t count = min_count; count <= max_count; count *= 2){
    values = std::vector<uint32_t>(count); 
    std::generate(values.begin(), values.end(), std::rand);
    values2 = values;
		time_average(run_prefix_par_no_cache_write_back_cache, count, "prefix_par_no_cache_write_back_cache");
		time_average(run_prefix_par_no_cache, count,                  "prefix_par_no_cache                 ");
		time_average(run_prefix_par, count,                           "prefix_par_(byte)                   ");
		time_average(run_prefix_par_short, count,                     "prefix_par_(short)                  ");
		time_average(run_prefix_par_nibble, count,                    "prefix_par_(nibble)                 ");
		time_average(run_par, count,                                  "par_(byte)                          ");
		time_average(run_par_nibble, count,                           "par_(nibble)                        ");
		time_average(run_par_short, count,                            "par_(short)                         ");
		time_average(run_seq, count,                                  "seq_(byte)                          ");
	}


  return 0;
}

