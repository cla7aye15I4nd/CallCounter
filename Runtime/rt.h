#ifndef RT_H
#define RT_H

extern "C"
{
  void __callcounter(const char *func_name);
  void __callcounter_report();
}

#endif // RT_H