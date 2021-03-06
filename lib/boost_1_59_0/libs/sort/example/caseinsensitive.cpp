// Example of sorting a struct using a case-insensitive string key.
//
//  Copyright Steven Ross 2009-2014.
//
// Distributed under the Boost Software License, Version 1.0.
//    (See accompanying file LICENSE_1_0.txt or copy at
//          http://www.boost.org/LICENSE_1_0.txt)

//  See http://www.boost.org/libs/sort for library home page.

#include <boost/algorithm/string.hpp>
#include <boost/sort/spreadsort/string_sort.hpp>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
#include <iostream>
#include <fstream>
#include <string>
using std::string;
using namespace boost::sort::spreadsort;

struct DATA_TYPE {
    string a;
};

struct lessthan {
  inline bool operator()(const DATA_TYPE &x, const DATA_TYPE &y) const { 
    return boost::algorithm::ilexicographical_compare(x.a, y.a);
  }
};

struct bracket {
  inline unsigned char operator()(const DATA_TYPE &x, size_t offset) const {
    return toupper(x.a[offset]);
  }
};

struct getsize {
  inline size_t operator()(const DATA_TYPE &x) const{ return x.a.size(); }
};

//Pass in an argument to test std::sort
int main(int argc, const char ** argv) {
  std::ifstream indata;
  std::ofstream outfile;
  bool stdSort = false;
  unsigned loopCount = 1;
  for (int u = 1; u < argc; ++u) {
    if (std::string(argv[u]) == "-std")
      stdSort = true;
    else
      loopCount = atoi(argv[u]);
  }
  double total = 0.0;
  //Run multiple loops, if requested
  std::vector<DATA_TYPE> array;
  for (unsigned u = 0; u < loopCount; ++u) {
    indata.open("input.txt", std::ios_base::in | std::ios_base::binary);  
    if (indata.bad()) {
      printf("input.txt could not be opened\n");
      return 1;
    }
    DATA_TYPE inval;
    indata >> inval.a;
    while (!indata.eof() ) {
      array.push_back(inval);
      indata >> inval.a;
    }
      
    indata.close();
    clock_t start, end;
    double elapsed;
    start = clock();
    if (stdSort)
      std::sort(array.begin(), array.end(), lessthan());
    else
      string_sort(array.begin(), array.end(), bracket(), getsize(), lessthan());
    end = clock();
    elapsed = static_cast<double>(end - start);
    if (stdSort)
      outfile.open("standard_sort_out.txt", std::ios_base::out |
                   std::ios_base::binary | std::ios_base::trunc);
    else
      outfile.open("boost_sort_out.txt", std::ios_base::out |
                   std::ios_base::binary | std::ios_base::trunc);
    if (outfile.good()) {
      for (unsigned u = 0; u < array.size(); ++u)
        outfile << array[u].a << "\n";
      outfile.close();
    }
    total += elapsed;
    array.clear();
  }
  if (stdSort)
    printf("std::sort elapsed time %f\n", total / CLOCKS_PER_SEC);
  else
    printf("spreadsort elapsed time %f\n", total / CLOCKS_PER_SEC);
  return 0;
}
