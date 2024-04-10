#include <common.h>
#include "syscall.h"
#include "fs.h"

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
  if (c->GPR2 == 1 || c->GPR2 == 2) {
    for (size_t i = 0; i < c->GPR4; ++i){
      putch(*((char *)(c->GPR3) + i));
    }
    c->GPRx = c->GPR4;
    return;
  }
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
      Log("SYS_open");
      strace(c);
      sys_open(c);
      break;
    case SYS_close:
      Log("SYS_close");
      strace(c);
      sys_close(c);
      break;
    case SYS_read:
      Log("SYS_read");   
      strace(c);   
      sys_read(c);
      break;
    case SYS_lseek:
      Log("SYS_lseek");
      strace(c);
      sys_lseek(c);
      break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
