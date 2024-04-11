#include <proc.h>
#include <elf.h>
#include "fs.h"

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
  int fd = fs_open(filename, 0, 0);

  Elf_Ehdr elf;

  fs_read(fd, &elf, sizeof(Elf_Ehdr));
  // check valid elf
  assert((*(uint32_t *)elf.e_ident == 0x464c457f));
	assert(elf.e_machine == EM_RISCV);
	Elf_Phdr phdr;
	//ELF中采用program header table来管理segment, 其一个表项描述了一个segment的所有属性
	//包括类型, 虚拟地址, 标志, 对齐方式, 以及文件内偏移量和segment大小. 
	//e_phnum Program header table entry count
	//e_phoff Program header table file offset 
  //e_phentsize Program header table entry size

  for (int i = 0; i < elf.e_phnum; i++) {
    uint32_t base = elf.e_phoff + i * elf.e_phentsize;
 
    fs_lseek(fd, base, 0);
    fs_read(fd, &phdr, elf.e_phentsize);
    
    // 装载segment
    if (phdr.p_type == PT_LOAD) {
 
      char * buf_malloc = (char *)malloc(phdr.p_filesz);
 
      fs_lseek(fd, phdr.p_offset, 0);
      assert(fs_read(fd, buf_malloc, phdr.p_filesz) == phdr.p_filesz);
      
      memcpy((void*)phdr.p_vaddr, buf_malloc, phdr.p_filesz);
      memset((void*)phdr.p_vaddr + phdr.p_filesz, 0, phdr.p_memsz - phdr.p_filesz);
      
      free(buf_malloc);
    }
  }

  fs_close(fd);
  return elf.e_entry;
}

//其中的pcb参数目前暂不使用, 可以忽略
void naive_uload(PCB *pcb, const char *filename) {
  uintptr_t entry = loader(pcb, filename);
  Log("Jump to entry = %p", entry);
  //jump to entry
  ((void(*)())entry) ();
}

