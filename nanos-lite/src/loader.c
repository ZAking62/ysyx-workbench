#include <proc.h>
#include <elf.h>

//usr/include//elf.h
#ifdef __LP64__
# define Elf_Ehdr Elf64_Ehdr
# define Elf_Phdr Elf64_Phdr
#else
# define Elf_Ehdr Elf32_Ehdr
# define Elf_Phdr Elf32_Phdr
#endif

size_t ramdisk_read(void *buf, size_t offset, size_t len); 
static uintptr_t loader(PCB *pcb, const char *filename) {
  Elf_Ehdr ehdr;
	//使用ramdisk_read函数从ramdisk中读取信息，从偏移量0开始读取sizeof(Elf_Ehdr)字节的数据。
  ramdisk_read(&ehdr, 0, sizeof(Elf_Ehdr));
  // check valid elf
  assert((*(uint32_t *)ehdr.e_ident == 0x464c457f));
	//assert(ehdr.e_machine == EM_RISCV);
	
	//ELF中采用program header table来管理segment, 其一个表项描述了一个segment的所有属性
	//包括类型, 虚拟地址, 标志, 对齐方式, 以及文件内偏移量和segment大小. 
	//e_phnum;    程序表的数量program header table entry count
  Elf_Phdr phdr[ehdr.e_phnum];
	//ehdr.e_phoff, Program header table file offset 
  ramdisk_read(phdr, ehdr.e_phoff, sizeof(Elf_Phdr)*ehdr.e_phnum);
  for (int i = 0; i < ehdr.e_phnum; i++) {
    if (phdr[i].p_type == PT_LOAD) {
			//读入segment
      ramdisk_read((void*)phdr[i].p_vaddr, phdr[i].p_offset, phdr[i].p_memsz);
			//.bss置零
      memset((void*)(phdr[i].p_vaddr+phdr[i].p_filesz), 0, phdr[i].p_memsz - phdr[i].p_filesz);
    }
  }
  return ehdr.e_entry;
}

//其中的pcb参数目前暂不使用, 可以忽略, 而因为ramdisk中目前只有一个文件, filename参数也可以忽略. 在下一个阶段实现文件系统之后, filename就派上用场了
void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  ((void(*)())entry) ();
}

