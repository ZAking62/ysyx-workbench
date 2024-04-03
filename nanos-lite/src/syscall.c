#include <common.h>
#include "syscall.h"

enum {
  SYS_exit,
  SYS_yield,
  SYS_open,
  SYS_read,
  SYS_write,
  SYS_kill,
  SYS_getpid,
  SYS_close,
  SYS_lseek,
  SYS_brk,
  SYS_fstat,
  SYS_time,
  SYS_signal,
  SYS_execve,
  SYS_fork,
  SYS_link,
  SYS_unlink,
  SYS_wait,
  SYS_times,
  SYS_gettimeofday
};

static void strace(Context *c){
  Log("System call trace\nmcause = %d GPR1 = %d GPR2 = %dGPR3 = %d GPR4 =%d",
      c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4);
}

void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

void sys_write(Context *c){
  assert(c->GPR2 == 1 || c->GPR2 == 2);
  for (int i = 0; i < c->GPR4; ++i){
		putch(*(((char *)c->GPR3) + i));
  }
  c->GPRx = c->GPR4;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; //a7
	strace(c);
	//参考应用的系统调用号
  switch (a[0]) {
    case SYS_exit:
			halt(0);
			break;
    case SYS_yield:
			sys_yield(c);
			break;
		case SYS_write:
			sys_write(c);
			break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
