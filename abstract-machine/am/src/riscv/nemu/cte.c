#include <am.h>
#include <riscv/riscv.h>
#include <klib.h>
#define IRQ_TIMER 0x80000007

static Context* (*user_handler)(Event, Context*) = NULL;

Context* __am_irq_handle(Context *c) {
  if (user_handler) {
    Event ev = {0};
		printf("mcause is %d\n", c->mcause);
    switch (c->mcause) {
			case 0:
			case 1:
			case 2:
			case 3:
			case 4:
			case 5:
			case 6:
      case 7:
			case 8:
			case 9:
			case 10:
			case 11:
				if(c->GPR1 == -1){
					ev.event = EVENT_YIELD; break;
				}
				ev.event = EVENT_SYSCALL; break;
      default: ev.event = EVENT_ERROR; break;
    }
		//处理事件，返回上下文
    c = user_handler(ev, c);
    assert(c != NULL);
  }

  return c;
}

extern void __am_asm_trap(void);

//当发生事件时, CTE将会把事件和相关的上下文作为参数, 来调用这个回调函数, 交由操作系统进行后续处理
bool cte_init(Context*(*handler)(Event, Context*)) {
  // initialize exception entry
	// 将异常入口地址设置到mtvec寄存器中,异常处理的入口地址设置为__am_asm_trap
	// abstract-machine/am/src/riscv/nemu/trap.S
  asm volatile("csrw mtvec, %0" : : "r"(__am_asm_trap));

  // register event handler
	// 保存用户提供的事件处理程序函数指针handler。
	// 这个事件处理程序将在事件发生时被调用，用于处理事件和返回上下文
  user_handler = handler;

  return true;
}

Context *kcontext(Area kstack, void (*entry)(void *), void *arg) {
  return NULL;
}

//将异常种类存放到a7寄存器中，以及发起自陷
//其中ecall会使得程序流程转到之前注册的异常处理入口函数中去执行,即__am_asm_trap
void yield() {
#ifdef __riscv_e
  asm volatile("li a5, -1; ecall");
#else
  asm volatile("li a7, -1; ecall");
#endif
}

bool ienabled() {
  return false;
}

void iset(bool enable) {
}
