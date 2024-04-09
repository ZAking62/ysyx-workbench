#include <common.h>
#include "syscall.h"
#include "fs.h"

// enum {
//   SYS_exit,
//   SYS_yield,
//   SYS_open,
//   SYS_read,
//   SYS_write,
//   SYS_kill,
//   SYS_getpid,
//   SYS_close,
//   SYS_lseek,
//   SYS_brk,
//   SYS_fstat,
//   SYS_time,
//   SYS_signal,
//   SYS_execve,
//   SYS_fork,
//   SYS_link,
//   SYS_unlink,
//   SYS_wait,
//   SYS_times,
//   SYS_gettimeofday
// };


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


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; //a7
	//参考应用的系统调用号
  switch (a[0]) {
    case SYS_exit:
			Log("SYS_exit");
			strace(c);
			halt(0);
			break;
    case SYS_yield:
			Log("SYS_yield");
			strace(c);
			sys_yield(c);
			break;
		case SYS_write:
			Log("SYS_write");
			strace(c);
			sys_write(c);
			break;
    case SYS_brk:
      Log("SYS_brk");
      strace(c);
      sys_brk(c);
      break;
    case SYS_open:
      sys_open(c);
      break;
    case SYS_close:
      sys_close(c);
      break;
    case SYS_read:
      sys_read(c);
      break;
    case SYS_lseek:
      sys_lseek(c);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
