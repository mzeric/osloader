#include "types.h"
#include "i386.h"
#include "multiboot.h"

#define null_desc                 (0x0000000000000000ULL)
#define code32_desc               (0x00cf9b000000ffffULL)
#define code64_desc               (0x00af9b000000ffffULL)
#define data32_desc               (0x00cf93000000ffffULL)

#define CS_SELECTOR  segment_selector(1, SEGMENT_SELECTOR_GDT, SEGMENT_SELECTOR_KERNEL)
#define CS64_SELECTOR  segment_selector(2, SEGMENT_SELECTOR_GDT, SEGMENT_SELECTOR_KERNEL)
#define DS_SELECTOR  segment_selector(3, SEGMENT_SELECTOR_GDT, SEGMENT_SELECTOR_KERNEL)

static segment_descriptor_t __attribute__((aligned(8)))          _gdt[] =
{
   { .raw = null_desc   },
   { .raw = code32_desc },
   { .raw = code64_desc },
   { .raw = data32_desc },
};



static pml4e_t    __attribute__((aligned(PAGE_SIZE)))  pml4[PML4E_PER_PML4];
static pdpe_t     __attribute__((aligned(PAGE_SIZE)))  pdp[PDPE_PER_PDP];
static pde64_t    __attribute__((aligned(PAGE_SIZE)))  pd[4][PDE64_PER_PD];


void setup_gdt(){
   gdt_reg_t gdtr;

   gdtr.limit = sizeof(_gdt) - 1;
   gdtr.addr  = &_gdt[0];

   set_gdtr(gdtr);

   segmem_reload(CS_SELECTOR, DS_SELECTOR);
}
/* dummy */
void setup_interrupt(){

}
void init_pagemem_1G()
{
   uint32_t p;

   for(p=0 ; p<4 ; p++)
      pg_set_large_entry(&pdp[p], PG_KRN|PG_RW, p);
}

void init_pagemem_2M()
{
   uint32_t base, p, n;

   for(p=0 ; p<4 ; p++)
   {
      base = (p<<PG_1G_SHIFT)>>PG_2M_SHIFT;
      pg_set_entry(&pdp[p], PG_KRN|PG_RW, page_nr(pd[p]));

      for(n=0 ; n<PDE64_PER_PD ; n++)
         pg_set_large_entry(&pd[p][n], PG_KRN|PG_RW, base+n);
   }
}

void setup_page()
{
   cr3_reg_t cr3;

   pg_set_entry(&pml4[0], PG_KRN|PG_RW, page_nr(pdp));

   init_pagemem_2M();

   cr3.raw = 0ULL;
   cr3.pml4.addr = page_nr(pml4);

   set_cr4(get_cr4()|CR4_PAE|CR4_PSE);
   set_cr3(cr3.low);
   lm_enable();
}
void enter_long_mode(multiboot_info_t *mbi)
{
   multiboot_module_t *mod;
   fptr32_t entry;

   mod = (multiboot_module_t*)mbi->mods_addr;
   elf_module_load(mod);

   entry.segment = CS64_SELECTOR;
   entry.offset  = (uint32_t)elf_module_entry(mod->mod_start);

   set_cr0(CR0_PG|CR0_ET|CR0_PE);
   set_reg(edi, mbi);
   farjump(entry);
}
int i386_cpu_init(){
	setup_gdt();
	setup_page();
	setup_interrupt();
}

