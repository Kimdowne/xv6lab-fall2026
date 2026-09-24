#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/sysinfo.h"
#include "user/user.h"

int
main(void)
{
  struct sysinfo info;

  if (sysinfo(&info) < 0) {
    printf("sysinfo failed\n");
    exit(1);
  }
  printf("freepages = %lu pages\n", info.freepages);
  printf("nproc     = %lu\n", info.nproc);

  int pid = fork();                              // 자식 프로세스를 하나 만든다.
  if (pid < 0) {                                 // 자식 생성 실패를 확인한다.
    printf("fork failed\n");                     // 실패 메시지를 출력한다.
    exit(1);                                     // 테스트를 실패로 종료한다.
  }                                              // 생성 실패 처리 끝.
  if (pid == 0) {                                // 자식 프로세스가 실행할 부분이다.
    exit(0);                                     // 종료 후 부모가 회수할 때까지 좀비로 남는다.
  }                                              // 자식 처리 끝.
  if (sysinfo(&info) < 0) {                       // 부모가 자식 회수 전의 정보를 조회한다.
    printf("sysinfo failed after fork\n");        // 조회 실패를 출력한다.
    wait(0);                                     // 생성한 자식을 회수한다.
    exit(1);                                     // 테스트를 실패로 종료한다.
  }                                              // 조회 실패 처리 끝.
  printf("after fork: nproc = %lu\n", info.nproc); // 자식이 포함된 프로세스 수를 출력한다.
  if (wait(0) < 0) {                             // 자식의 종료를 기다리고 슬롯을 회수한다.
    printf("wait failed\n");                     // 회수 실패를 출력한다.
    exit(1);                                     // 테스트를 실패로 종료한다.
  }                                              // 회수 실패 처리 끝.
  if (sysinfo(&info) < 0) {                       // 자식 회수 후의 정보를 다시 조회한다.
    printf("sysinfo failed after wait\n");        // 조회 실패를 출력한다.
    exit(1);                                     // 테스트를 실패로 종료한다.
  }                                              // 조회 실패 처리 끝.
  printf("after wait: nproc = %lu\n", info.nproc); // 자식이 제외된 프로세스 수를 출력한다.
  exit(0);
}