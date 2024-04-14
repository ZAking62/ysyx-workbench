#include <common.h>
#include "syscall.h"
#include "fs.h"
#include <sys/time.h>

static void strace(Context *c){
  Log("System call trace\nirqtype = %d syscalltype = %d arg1 = %d arg2 = %d arg3 = %d ret = %d",
      c->mcause, c->GPR1, c->GPR2, c->GPR3, c->GPR4, c->GPRx);
}

void sys_yield(Context *c){
  yield();
  c->GPRx = 0;
}

void sys_read(Context *c){
  int ret = fs_read(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_write(Context *c){
  int ret = fs_write(c->GPR2, (void *)c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_brk(Context *c){
  c->GPRx = 0;
}

void sys_open(Context *c){
  int ret = fs_open((char *)c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_close(Context *c){
  int ret = fs_close(c->GPR2);
  c->GPRx = ret;
}

void sys_lseek(Context *c){
  int ret = fs_lseek(c->GPR2, c->GPR3, c->GPR4);
  c->GPRx = ret;
}

void sys_gettimeofday(Context* c){
  struct timeval *tv = (struct timeval *)(c->GPR2);
  uint64_t time = io_read(AM_TIMER_UPTIME).us;
  tv->tv_sec = (time / 1000000);
  tv->tv_usec = (time % 1000000);
  c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; //a7
	//参考应用的系统调用号
  strace(c);
  switch (a[0]) {
    case SYS_exit:
			// Log("SYS_exit");
			halt(0);
			break;
    case SYS_yield:
			// Log("SYS_yield");
			sys_yield(c);
			break;
		case SYS_write:
			// Log("SYS_write");
			sys_write(c);
			break;
    case SYS_brk:
      // Log("SYS_brk");
      sys_brk(c);
      break;
    case SYS_open:
      // Log("SYS_open");
      sys_open(c);
      break;
    case SYS_close:
      // Log("SYS_close");
      sys_close(c);
      break;
    case SYS_read:
      // Log("SYS_read");   
      sys_read(c);
      break;
    case SYS_lseek:
      // Log("SYS_lseek");
      sys_lseek(c);
      break;
    case SYS_gettimeofday:
      //Log("SYS_gettimeofday");
      sys_gettimeofday(c);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
