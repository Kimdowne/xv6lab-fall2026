#include "kernel/types.h"
#include "kernel/stat.h"
#include "user/user.h"

int
main(void)
{
  printf("free pages: %lu\n", freepages());
  exit(0);
}