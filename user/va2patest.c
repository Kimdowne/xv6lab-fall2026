#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int global = 42;                      // 데이터 영역에 놓인다

void
show(char *name, void *va)
{
  printf("%s  va %p -> pa %p\n", name, va, (void *)va2pa((uint64)va));
}

int
main(void)
{
  int local = 7;                      // 스택에 놓인다

  show("text ", (void *)main);        // 함수의 기계어가 놓인 곳 = 코드 영역
  show("data ", &global);
  show("stack", &local);

  exit(0);
}
