#include <common.h>
#include "syscall.h"

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1; //a7

	//参考应用的系统调用号
  switch (a[0]) {
    case 0:
			Log("SYS_exit, GPRx = %d", c->GPRx);
			//c->GPRx=0;
			halt(c->GPRx);//SYS_exit系统调用
    case 1:
			Log("SYS_yield");
			yield(); //SYS_yield系统调用
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
