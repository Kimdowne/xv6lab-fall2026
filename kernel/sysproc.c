#include "types.h"
#include "riscv.h"
#include "defs.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "proc.h"
#include "vm.h"

uint64
sys_exit(void)
{
  int n;
  argint(0, &n);
  kexit(n);
  return 0; // not reached
}

uint64
sys_getpid(void)
{
  return myproc()->pid;
}

uint64
sys_fork(void)
{
  return kfork();
}

uint64
sys_wait(void)
{
  uint64 p;
  argaddr(0, &p);
  return kwait(p);
}

uint64
sys_sbrk(void)
{
  uint64 addr;
  int t;
  int n;

  argint(0, &n);
  argint(1, &t);
  addr = myproc()->sz;

  if (t == SBRK_EAGER || n < 0) {
    if (growproc(n) < 0) {
      return -1;
    }
  } else {
    // Lazily allocate memory for this process: increase its memory
    // size but don't allocate memory. If the processes uses the
    // memory, vmfault() will allocate it.
    if (addr + n < addr)
      return -1;
    if (addr + n > TRAPFRAME)
      return -1;
    myproc()->sz += n;
  }
  return addr;
}

uint64
sys_pause(void)
{
  int n;
  uint ticks0;

  argint(0, &n);
  if (n < 0)
    n = 0;
  acquire(&tickslock);
  ticks0 = ticks;
  while (ticks - ticks0 < n) {
    if (killed(myproc())) {
      release(&tickslock);
      return -1;
    }
    sleep_prepare(&ticks);
    release(&tickslock);
    sleep();
    acquire(&tickslock);
  }
  release(&tickslock);
  return 0;
}

uint64
sys_kill(void)
{
  int pid;

  argint(0, &pid);
  return kkill(pid);
}

// return how many clock tick interrupts have occurred
// since start.
uint64
sys_uptime(void)
{
  uint xticks;

  acquire(&tickslock);
  xticks = ticks;
  release(&tickslock);
  return xticks;
}

uint64
sys_va2pa(void)
{
  uint64 va;
  pte_t *pte;
  struct proc *p = myproc();         // 호출한 프로세스의 페이지 테이블을 사용한다

  argaddr(0, &va);                    // 첫 번째 인자의 주소 값만 읽고 메모리는 역참조하지 않는다
  if (va >= MAXVA)
    return 0;                         // 이대로 walk 에 주면 커널이 panic 한다

  // 주소가 거치는 L2 L1 L0 칸 번호와 페이지 내부 오프셋을 출력한다.
  // PX는 각 단계의 9비트 인덱스를, 아래 12비트는 4KiB 페이지 안의 위치를 뜻한다.
  // 출력 형식에 맞춰 칸 번호는 int로, 오프셋은 unsigned int로 변환한다.
  printk("va %p : L2=%d L1=%d L0=%d off=0x%x\n",
         (void *)va,
         (int)PX(2, va),
         (int)PX(1, va),
         (int)PX(0, va),
         (unsigned int)(va & 0xFFF));

  pte = walk(p->pagetable, va, 0);    // alloc = 0 : 찾기만 하고 만들지 않는다
  // 중간 테이블이 없거나 마지막 PTE가 유효하지 않으면 변환할 수 없다.
  if (pte == 0 || (*pte & PTE_V) == 0)
    return 0;                         // 매핑이 없다
  if ((*pte & PTE_U) == 0)
    return 0;                         // 트랩프레임 등 커널 전용 매핑은 제외한다

  // PTE에서 얻은 물리 페이지 시작 주소에 페이지 내부 오프셋을 더한다.
  // PTE2PA는 PTE의 플래그를 제거하고 물리 페이지 번호를 주소로 복원한다.
  return PTE2PA(*pte) + (va & 0xFFF);
}
