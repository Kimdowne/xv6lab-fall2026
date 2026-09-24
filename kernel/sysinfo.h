// kernel/sysinfo.h
struct sysinfo {
  uint64 freepages;   // 남은 페이지 수
  uint64 nproc;       // UNUSED 가 아닌 프로세스 수
};
