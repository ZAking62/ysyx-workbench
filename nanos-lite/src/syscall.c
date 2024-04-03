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
  Log("System call trace\nmcause\t\tGPR1\t\tGPR2\t\tGPR3\t\tGPR4 \n0x%x\t%d\t\t0x%x\t\t0x%x\t\t0x%x",
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
	Log("a7 = %d", a[0]);
	strace(c);
	//参考应用的系统调用号
  switch (a[0]) {
    case SYS_exit:
			halt(0);
			break;
    case SYS_yield:
			Log("SYS_yield, GPRx = %d", c->GPRx);
			sys_yield(c);
			Log("SYS_yield, GPRx = %d", c->GPRx);
			break;
		case SYS_write:
			sys_write(c);
			Log("SYS_write, GPRx = %d", c->GPRx);
			break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
