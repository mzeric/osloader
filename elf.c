#include "elf.h"
#include "types.h"
#include "multiboot.h"

#define printf
#define exit
#define perror
#define memset
#define open
#define mmap
#define fstat
void panic(const char * str){
	perror(str);
	exit(1);
}
/*
** This code works under 32 or 64 bits
** However, 32 bits code truncates offsets
** that can not fit into 32 bits addresses
** by making use of "offset_t" which is
** "unsigned long" (4 bytes under 32 bits).
*/
/*
Elf64 struct references:
	https://www.cs.stevens.edu/~jschauma/631/elf.html
*/
#define MULTIBOOT_HDR  __attribute__((section(".multiboot"),aligned(4)))
MULTIBOOT_HDR uint32_t  mbh_magic = MULTIBOOT_HEADER_MAGIC;
MULTIBOOT_HDR uint32_t  mbh_flags = MULTIBOOT_INFO_MEMORY | MULTIBOOT_INFO_BOOTDEV;
MULTIBOOT_HDR int32_t   mbh_chksm = -((MULTIBOOT_INFO_MEMORY | MULTIBOOT_INFO_BOOTDEV ) + MULTIBOOT_HEADER_MAGIC);
 

static Elf64_Shdr* __elf_section(Elf64_Ehdr *e_hdr, Elf64_Word type)
{
   Elf64_Half n;
   Elf64_Shdr *s_hdr;
   loc_t      sloc;

   sloc.linear = (offset_t)e_hdr + (offset_t)e_hdr->e_shoff;
   for(n=0 ; n<e_hdr->e_shnum ; n++)
   {
      s_hdr = (Elf64_Shdr*)sloc.addr;

      if(s_hdr->sh_type == type)
         return s_hdr;

      sloc.linear += (offset_t)e_hdr->e_shentsize;
   }

   return (Elf64_Shdr*)0;
}
void *all_section(uint64_t elf_addr){

   Elf64_Ehdr *e_hdr;
   Elf64_Phdr *p_hdr;
   Elf64_Shdr *b_hdr;
   int64_t   pad;
   uint64_t   linear;

   e_hdr = (Elf64_Ehdr*)((offset_t)elf_addr);
   p_hdr = (Elf64_Phdr*)((offset_t)e_hdr + (offset_t)e_hdr->e_phoff);

   printf(" %x == %x\n", e_hdr->e_machine, EM_X86_64);
   if(e_hdr->e_machine != EM_X86_64 || ! e_hdr->e_phnum || p_hdr->p_type != PT_LOAD)
      panic("invalid elf module");


   linear = (offset_t)p_hdr->p_vaddr;
   Elf64_Half n;
   Elf64_Shdr *s_hdr;
   linear = (offset_t)e_hdr + (offset_t)e_hdr->e_shoff;
   printf("copy %x <- %x size:%x\n", 0, p_hdr->p_offset, p_hdr->p_filesz);
   for(n=0 ; n<e_hdr->e_shnum ; n++)
   {
      s_hdr = (Elf64_Shdr*)linear;

      int type  = s_hdr->sh_type;
      printf("section[%d]: type(%X) flags(%X) addr(%x) offset(%x) size(%x)\n", n, 
      		       type,s_hdr->sh_flags, s_hdr->sh_addr, s_hdr->sh_offset, s_hdr->sh_size);

      linear += (offset_t)e_hdr->e_shentsize;
   }

}
static void __elf_module_load(uint32_t *mod_start, uint32_t *mod_end, offset_t base)
{
   Elf64_Ehdr *e_hdr;
   Elf64_Phdr *p_hdr;
   Elf64_Shdr *b_hdr;
   int64_t   pad;
   loc_t      file, mem;

   e_hdr = (Elf64_Ehdr*)((offset_t)mod_start);
   p_hdr = (Elf64_Phdr*)((offset_t)e_hdr + (offset_t)e_hdr->e_phoff);

   if(e_hdr->e_machine != EM_X86_64 || ! e_hdr->e_phnum || p_hdr->p_type != PT_LOAD){
      panic("invalid elf module");
      return; // in case panic is dummy
   }

   mem.linear = base + (offset_t)p_hdr->p_vaddr;
   
   if(mem.linear < (offset_t)mod_end){
      panic("elf is too big, reloate it and try again");
      return; // in case panic is dummy
   }
   

   file.linear = (offset_t)mod_start + (offset_t)p_hdr->p_offset;
   memcpy(mem.addr, file.addr, (offset_t)p_hdr->p_filesz);

   pad = p_hdr->p_memsz - p_hdr->p_filesz;
   if(pad > 0)
   {
      mem.linear += (offset_t)p_hdr->p_filesz;
      memset(mem.addr, 0, (offset_t)pad);
   }

   b_hdr = __elf_section(e_hdr, SHT_NOBITS);
   if(b_hdr)
   {
      mem.linear = base + (offset_t)b_hdr->sh_addr;
      memset(mem.addr, 0, (offset_t)b_hdr->sh_size);
   }
}

void elf_module_load(multiboot_module_t *mod)
{
   __elf_module_load(mod->mod_start, mod->mod_end, 0);
}
void bin_module_load(multiboot_module_t *mod, offset_t off_addr){
	offset_t size = (offset_t)mod->mod_end - (offset_t)mod->mod_start;	
	memcpy(off_addr, (offset_t)mod->mod_start, size);
}


Elf64_Addr elf_module_entry(uint64_t *mod_start)
{
   Elf64_Ehdr *e_hdr = (Elf64_Ehdr*)((offset_t)mod_start);
   return e_hdr->e_entry;
}

Elf64_Phdr* elf_module_phdr(uint64_t *mod_start)
{
   Elf64_Ehdr *e_hdr;
   Elf64_Phdr *p_hdr;

   e_hdr = (Elf64_Ehdr*)((offset_t)mod_start);
   p_hdr = (Elf64_Phdr*)((offset_t)e_hdr + (offset_t)e_hdr->e_phoff);

   if(e_hdr->e_phnum && p_hdr->p_type == PT_LOAD)
      return p_hdr;

   panic("invalid elf phdr");
   return (Elf64_Phdr*)0;
}

Elf64_Xword elf_module_load_size(uint64_t *mod)
{
   Elf64_Phdr *p_hdr = elf_module_phdr(mod);
   return p_hdr->p_memsz;
}
