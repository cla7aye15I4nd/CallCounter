#include "rt.h"
#include <unordered_map>
#include <string>
#include <iostream>
#include <fstream>

static std::unordered_map<char*, int> __callcounter_map;

extern "C"
{
  void __callcounter(char *name)
  {
    __callcounter_map[name]++;
  }

  void __callcounter_report()
  {
    std::ofstream report;
    report.open("call_counter_report.txt");

    for (auto &kv : __callcounter_map)
      report << *kv.first << ": " << kv.second << std::endl;

    report.close();
  }
}