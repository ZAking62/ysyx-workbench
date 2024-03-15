/***************************************************************************************
* Copyright (c) 2014-2022 Zihao Yu, Nanjing University
*
* NEMU is licensed under Mulan PSL v2.
* You can use this software according to the terms and conditions of the Mulan PSL v2.
* You may obtain a copy of Mulan PSL v2 at:
*          http://license.coscl.org.cn/MulanPSL2
*
* THIS SOFTWARE IS PROVIDED ON AN "AS IS" BASIS, WITHOUT WARRANTIES OF ANY KIND,
* EITHER EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO NON-INFRINGEMENT,
* MERCHANTABILITY OR FIT FOR A PARTICULAR PURPOSE.
*
* See the Mulan PSL v2 for more details.
***************************************************************************************/

#include <isa.h>

//我们可以假设CPU有一条虚构的指令raise_intr, 执行这条虚构指令的行为就是上文提到的异常响应过程. 
//显然, 这一行为是可以用状态机视角来描述的, 例如在riscv32中可以表示成:
//SR[mepc] <- PC  将PC值保存到mepc寄存器
//SR[mcause] <- 一个描述失败原因的号码   设置mcause异常号
//PC <- SR[mtvec]		从mtvec寄存器中取出异常入口地址，跳转到异常入口地址
//抛出一个号码为NO的异常, 其中epc为触发异常的指令PC, 返回异常处理的入口地址.
word_t isa_raise_intr(word_t NO, vaddr_t epc) {
  /* TODO: Trigger an interrupt/exception with ``NO''.
   * Then return the address of the interrupt/exception vector.
   */
  if(NO==0){
    epc+=4;
  }
	cpu.csr.mcause = NO;
  cpu.csr.mepc = epc;

  return cpu.csr.mtvec;
}

//查询当前是否有未处理的中断, 若有则返回中断号码, 否则返回INTR_EMPTY.
word_t isa_query_intr() {
  return INTR_EMPTY;
}


