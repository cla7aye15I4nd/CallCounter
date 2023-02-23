#include <stdio.h>

#define FN(x, y)                     \
  void __attribute__((noinline)) x() \
  {                                  \
    printf(#x "\n");                 \
    y();                             \
    y();                             \
  }


void d()
{
  printf("d\n");
}

FN(c, d)
FN(b, c)
FN(a, b)

int main()
{
  a();
  return 0;
}