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

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; //a7
	Log("a7 = %d", a[0]);

	//参考应用的系统调用号
  switch (a[0]) {
    case SYS_exit:
			Log("SYS_exit, GPRx = %d", c->GPRx);
			//c->GPRx=0;
			halt(c->GPRx);//SYS_exit系统调用
    case SYS_yield:
			Log("SYS_yield, GPRx = %d", c->GPRx);
			yield(); //SYS_yield系统调用
		case SYS_write:
			Log("SYS_write, GPRx = %d", c->GPRx);
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
