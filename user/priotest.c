#include "kernel/types.h"
#include "user/user.h"

// [3번 질문]: 강의자료의 코드 참조
// 사용법: priotest [A] [B] [C]  (기본값1 10 10)
// 같은 계산을 수행하는 자식 프로세스 세 개를 만든다.
#define N 3

int
main(int argc, char *argv[])
{
  // 인자가 없으면 A=1, B=10, C=10을 사용한다.
  int prio[N] = {1, 10, 10};

  // 명령줄에서 지정한 우선순위를 기본값에 덮어씀.
  for (int i = 0; i < N && i + 1 < argc; i++)
    prio[i] = atoi(argv[i + 1]);

  // 테스트 시작 시각을 기록한다.
  int start = uptime();

  for (int i = 0; i < N; i++) {
    if (fork() == 0) {
      // 각 자식이 자신의 우선순위를 설정.
      set_priority(prio[i]);

      // 세 자식 모두 같은 양의 계산을 수행.
      volatile unsigned long x = 0;
      for (unsigned long j = 0; j < 1500000000UL; j++)
        x += j;

      // CPU 사용량이 아니라 시작 후 종료까지의 경과 틱을 출력.
      printf("%c (pid %d, prio %d) done at %d ticks\n",
             'A' + i, getpid(), prio[i], uptime() - start);

      exit(0);
    }
  }

  // 부모는 세 자식의 종료를 기다림.
  for (int i = 0; i < N; i++)
    wait(0);

  exit(0);
}