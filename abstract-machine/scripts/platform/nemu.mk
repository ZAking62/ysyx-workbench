##添加am与平台相关的sources
AM_SRCS := platform/nemu/trm.c \
           platform/nemu/ioe/ioe.c \
           platform/nemu/ioe/timer.c \
           platform/nemu/ioe/input.c \
           platform/nemu/ioe/gpu.c \
           platform/nemu/ioe/audio.c \
           platform/nemu/ioe/disk.c \
           platform/nemu/mpe.c

###编译flags
CFLAGS    += -fdata-sections -ffunction-sections

###defsym为程序中的标志pmem_start设定地址,_pmem_start这个标志是在nemu.h中声明的
LDFLAGS   += -T $(AM_HOME)/scripts/linker.ld \
             --defsym=_pmem_start=0x80000000 --defsym=_entry_offset=0x0

###在链接过程中用-e指定程序的入口
##程序入口是_start函数，被定义在am/src/riscv/nemu/start.S文件中, _start函数会调用_trm_init函数
LDFLAGS   += --gc-sections -e _start

##模拟器运行参数NEMUFLAGS
NEMUFLAGS += -l $(shell dirname $(IMAGE).elf)/nemu-log.txt -b

CFLAGS += -DMAINARGS=\"$(mainargs)\"
CFLAGS += -I$(AM_HOME)/am/src/platform/nemu/include
.PHONY: $(AM_HOME)/am/src/platform/nemu/trm.c

###添加image、run, gdb等运行目标
###image目标是将ELF反汇编以及生成最终的镜像文件,用于实现ftrace
image: $(IMAGE).elf
	@$(OBJDUMP) -d $(IMAGE).elf > $(IMAGE).txt
	@echo + OBJCOPY "->" $(IMAGE_REL).bin
	@$(OBJCOPY) -S --set-section-flags .bss=alloc,contents -O binary $(IMAGE).elf $(IMAGE).bin

run: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) run ARGS="$(NEMUFLAGS)" IMG=$(IMAGE).bin

gdb: image
	$(MAKE) -C $(NEMU_HOME) ISA=$(ISA) gdb ARGS="$(NEMUFLAGS)" IMG=$(IMAGE).bin
