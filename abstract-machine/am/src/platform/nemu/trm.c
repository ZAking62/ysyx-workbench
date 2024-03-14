#include <am.h>
#include <nemu.h>

extern char _heap_start;
int main(const char *args);

//Area heap结构用于指示堆区的起始和末尾
Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif

//传入MAINARGS=“”作为mainargs,32line
static const char mainargs[] = MAINARGS;

//用于输出一个字符,输出到设备(串口)
void putch(char ch) {
	//outb定义abstract-machine/am/src/riscv/riscv.h
  outb(SERIAL_PORT, ch);
}

//用于结束程序的运行
//halt是对不同架构的抽象，nemu_trap是cpu的功能
void halt(int code) {
  nemu_trap(code);

  // should not reach here
  while (1);
}


//用于进行TRM相关的初始化工作
void _trm_init() {
  int ret = main(mainargs);
  halt(ret);
}
