/*******************************************************************************
 * tests/sort_strings_test.cpp
 *
 * String sorting test program
 *
 * Part of tlx - http://panthema.net/tlx
 *
 * Copyright (C) 2015-2019 Timo Bingmann <tb@panthema.net>
 *
 * All rights reserved. Published under the Boost Software License, Version 1.0
 ******************************************************************************/

#include "sort_strings_test.hpp"

#include <tlx/sort/strings/parallel_radixsort.hpp>
#include <tlx/sort/strings_parallel.hpp>

#include <tlx/sort/strings.hpp>

#include <tlx/sort/strings/string_ptr.hpp>

#include <tlx/logger.hpp>
#include <tlx/simple_vector.hpp>
#include <tlx/timestamp.hpp>

#include <chrono>
#include <random>

using namespace tlx::sort_strings_detail;

void TestFrontend(const size_t num_strings, const size_t num_chars,
                  const std::string& letters) {

    std::default_random_engine rng(seed);

    LOG1 << "Running sort_strings() on " << num_strings
         << " uint8_t* strings";

    // array of string pointers
    tlx::simple_vector<uint8_t*> cstrings(num_strings);

    // generate random strings of length num_chars
    for (size_t i = 0; i < num_strings; ++i)
    {
        size_t slen = num_chars + (rng() >> 8) % (num_chars / 4);

        cstrings[i] = new uint8_t[slen + 1];
        fill_random(rng, letters, cstrings[i], cstrings[i] + slen);
        cstrings[i][slen] = 0;
    }

    // run sorting algorithm
    {
        double ts1 = tlx::timestamp();

        UCharStringSet ss(cstrings.data(), cstrings.data() + num_strings);

        // tlx::sort_strings(cstrings.data(), num_strings, /* memory */ 0);
        parallel_radix_sort_8bit_generic<PRSParametersDefault, UCharStringSet>(ss);

        double ts2 = tlx::timestamp();
        LOG1 << "sorting took " << ts2 - ts1 << " seconds";

        // check result
        if (!ss.check_order())
        {
            LOG1 << "Result is not sorted!";
            abort();
        }
    }

    // free memory.
    for (size_t i = 0; i < num_strings; ++i)
        delete[] cstrings[i];
}

void test_all(const size_t num_strings) {

    if (num_strings <= 1024) {
        run_tests(insertion_sort);
    }

    TestFrontend(num_strings, 16, letters_alnum);
}

int main() {
    // run tests
    test_all(16);
    test_all(256);
    test_all(65550);
    if (tlx_more_tests) {
        test_all(1024 * 1024);
        test_all(16 * 1024 * 1024);
    }

    return 0;
}

/******************************************************************************/
