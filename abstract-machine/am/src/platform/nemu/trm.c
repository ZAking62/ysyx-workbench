#include <am.h>
#include <nemu.h>

extern char _heap_start;
int main(const char *args);

//abstract-machine/am/include/am.h列出了AM中的所有API
//abstract-machine/am/ 不同架构的AM API实现, 目前只需要关注NEMU相关的内容.
//每个架构都按照它们的特性实现这组API; 应用程序只需要直接调用这组API即可
//(在NEMU中)实现硬件功能 -> (在AM中)提供运行时环境 -> (在APP层)运行程序
//Area heap结构用于指示堆区的起始和末尾
Area heap = RANGE(&_heap_start, PMEM_END);
#ifndef MAINARGS
#define MAINARGS ""
#endif

//传入MAINARGS=“”作为mainargs,32line
static const char mainargs[] = MAINARGS;

//用于输出一个字符
void putch(char ch) {
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
