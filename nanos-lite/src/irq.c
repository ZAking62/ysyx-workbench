#include <common.h>
void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
  switch (e.event) {
		case 1:
			Log("EVENT_YIELD");
			//你需要在其中识别出自陷事件EVENT_YIELD, 然后输出一句话即可, 目前无需进行其它操作
			break;
		case 2:
			//Log("EVENT_SYSCALL");
			do_syscall(c);
			break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
