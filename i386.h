#ifndef _I386_HDR
#define _I386_HDR

#include "types.h"

typedef union {
   uint64_t raw;

   struct
   {
      uint64_t    limit_1:16;          /* bits 00-15 of the segment limit */
      uint64_t    base_1:16;           /* bits 00-15 of the base address */
      uint64_t    base_2:8;            /* bits 16-23 of the base address */
      uint64_t    type:4;              /* segment type */
      uint64_t    s:1;                 /* descriptor type */
      uint64_t    dpl:2;               /* descriptor privilege level */
      uint64_t    p:1;                 /* segment present flag */
      uint64_t    limit_2:4;           /* bits 16-19 of the segment limit */
      uint64_t    avl:1;               /* available for fun and profit */
      uint64_t    l:1;                 /* longmode */
      uint64_t    d:1;                 /* default length, depend on seg type */
      uint64_t    g:1;                 /* granularity */
      uint64_t    base_3:8;            /* bits 24-31 of the base address */

   } __attribute__((packed));

} __attribute__((packed)) segment_descriptor_t;


/*
** Global descriptor table
*/
typedef struct {
   uint16_t            limit;           /* dt limit = size - 1 */
	uint64_t 	addr;
} __attribute__((packed)) gdt_reg_t;

typedef uint16_t segment_selector_t;
#define SEGMENT_SELECTOR_GDT    0
#define SEGMENT_SELECTOR_LDT    1

#define SEGMENT_SELECTOR_KERNEL    0
#define SEGMENT_SELECTOR_USER    3

#define segment_selector(idx,ti,rpl)      (uint16_t)(((idx)<<3)|((ti)<<2)|(rpl))


/*
** Interrupt descriptor
*/
typedef union {
   uint64_t raw;

   struct
   {
      uint64_t  offset_0_15:16;    /* bits 00-15 of the isr offset */
      uint64_t  selector:16;    /* isr segment selector */
      uint64_t  ist:3;          /* stack table: only 64 bits */
      uint64_t  unused_1:5;       /* must be 0 */
      uint64_t  type:4;         /* interrupt/trap gate */
      uint64_t  unused_2:1;       /* must be zero */
      uint64_t  dpl:2;          /* privilege level */
      uint64_t  p:1;            /* present flag */
      uint64_t  offset_16_64:48;    /* bits 16-31 of the isr offset */
   	  uint64_t unused_3:32;

   } __attribute__((packed));
} __attribute__((packed)) interrupt_descriptor_t;

/*
** Interrupt descriptor table
*/
typedef struct {
   uint16_t        limit;           /* dt limit = size - 1 */
	uint64_t 		addr;
} __attribute__((packed)) idt_reg_t;

#define set_reg(_r_,_v_)  asm volatile ("mov %0, %%"#_r_::"m"(_v_):"memory")
#define set_gdtr(val)             asm volatile ("lgdt  %0"::"m"(val):"memory")
#define set_ldtr(val)             asm volatile ("lldt  %0"::"m"(val):"memory")
#define set_idtr(val)             asm volatile ("lidt  %0"::"m"(val):"memory")
#define set_tr(val)               asm volatile ("ltr   %%ax"::"a"(val))
#define farjump(_fptr)            asm volatile ("ljmp  *%0"::"m"(_fptr):"memory");




#define segmem_reload(_cs,_ds)                  \
   ({                                           \
		asm volatile ("ljmp  %0, $1f ; 1:"::"i"(_cs));	\
	    asm volatile (                          \
         "movw   %%ax, %%ss  \n"                \
         "movw   %%ax, %%ds  \n"                \
         "movw   %%ax, %%es  \n"                \
         "movw   %%ax, %%fs  \n"                \
         "movw   %%ax, %%gs  \n"                \
         ::"a"(_ds));                           \
   })



#define CR0_PE_BIT      0
#define CR0_MP_BIT      1
#define CR0_EM_BIT      2
#define CR0_TS_BIT      3
#define CR0_ET_BIT      4
#define CR0_NE_BIT      5
#define CR0_WP_BIT     16
#define CR0_AM_BIT     18
#define CR0_NW_BIT     29
#define CR0_CD_BIT     30
#define CR0_PG_BIT     31

#define CR0_PE         (1UL<<CR0_PE_BIT)
#define CR0_MP         (1UL<<CR0_MP_BIT)
#define CR0_EM         (1UL<<CR0_EM_BIT)
#define CR0_TS         (1UL<<CR0_TS_BIT)
#define CR0_ET         (1UL<<CR0_ET_BIT)
#define CR0_NE         (1UL<<CR0_NE_BIT)
#define CR0_WP         (1UL<<CR0_WP_BIT)
#define CR0_AM         (1UL<<CR0_AM_BIT)
#define CR0_NW         (1UL<<CR0_NW_BIT)
#define CR0_CD         (1UL<<CR0_CD_BIT)
#define CR0_PG         (1UL<<CR0_PG_BIT)

typedef union control_register_0
{
   struct
   {
      uint64_t    pe:1;   /* protected mode */
      uint64_t    mp:1;   /* monitor copro */
      uint64_t    em:1;   /* emulation */
      uint64_t    ts:1;   /* task switch */
      uint64_t    et:1;   /* ext type */
      uint64_t    ne:1;   /* num error */
      uint64_t    r1:10;  /* reserved */
      uint64_t    wp:1;   /* write protect */
      uint64_t    r2:1;   /* reserved */
      uint64_t    am:1;   /* align mask */
      uint64_t    r3:10;  /* reserved */
      uint64_t    nw:1;   /* not write through */
      uint64_t    cd:1;   /* cache disable */
      uint64_t    pg:1;   /* paging */

   } __attribute__((packed));

   raw64_t;

} __attribute__((packed)) cr0_reg_t;
typedef union control_register_3
{
   struct
   {
      uint64_t  r1:3;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  r2:7;
      uint64_t  addr:20;

   } __attribute__((packed));

   struct
   {
      uint64_t  r1:3;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  addr:27;

   } __attribute__((packed)) pae;

   struct
   {
      uint64_t  r1:3;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  r2:7;
      uint64_t  addr:40; /* bit 12 */

   } __attribute__((packed)) pml4;

   raw64_t;

} __attribute__((packed)) cr3_reg_t;

#define CR4_VME_BIT          0
#define CR4_PVI_BIT          1
#define CR4_TSD_BIT          2
#define CR4_DE_BIT           3
#define CR4_PSE_BIT          4
#define CR4_PAE_BIT          5
#define CR4_MCE_BIT          6
#define CR4_PGE_BIT          7
#define CR4_PCE_BIT          8
#define CR4_OSFXSR_BIT       9
#define CR4_OSXMMEXCPT_BIT  10
#define CR4_VMXE_BIT        13
#define CR4_SMXE_BIT        14
#define CR4_PCIDE_BIT       17
#define CR4_OSXSAVE_BIT     18
#define CR4_SMEP_BIT        20

#define CR4_VME             (1UL<<CR4_VME_BIT)
#define CR4_PVI             (1UL<<CR4_PVI_BIT)
#define CR4_TSD             (1UL<<CR4_TSD_BIT)
#define CR4_DE              (1UL<<CR4_DE_BIT)
#define CR4_PSE             (1UL<<CR4_PSE_BIT)
#define CR4_PAE             (1UL<<CR4_PAE_BIT)
#define CR4_MCE             (1UL<<CR4_MCE_BIT)
#define CR4_PGE             (1UL<<CR4_PGE_BIT)
#define CR4_PCE             (1UL<<CR4_PCE_BIT)
#define CR4_OSFXSR          (1UL<<CR4_OSFXSR_BIT)
#define CR4_OSXMMEXCPT      (1UL<<CR4_OSXMMEXCPT_BIT)
#define CR4_VMXE            (1UL<<CR4_VMXE_BIT)
#define CR4_SMXE            (1UL<<CR4_SMXE_BIT)
#define CR4_PCIDE           (1UL<<CR4_PCIDE_BIT)
#define CR4_OSXSAVE         (1UL<<CR4_OSXSAVE_BIT)
#define CR4_SMEP            (1UL<<CR4_SMEP_BIT)

typedef union control_register_4
{
   struct
   {
      uint64_t    vme:1;        /* virtual 8086 */
      uint64_t    pvi:1;        /* pmode virtual int */
      uint64_t    tsd:1;        /* time stamp disable */
      uint64_t    de:1;         /* debug ext */
      uint64_t    pse:1;        /* page sz ext */
      uint64_t    pae:1;        /* phys addr ext */
      uint64_t    mce:1;        /* machine check enable */
      uint64_t    pge:1;        /* page global enable */
      uint64_t    pce:1;        /* perf counter enable */
      uint64_t    osfxsr:1;     /* fxsave fxstore */
      uint64_t    osxmmexcpt:1; /* simd fpu excpt */
      uint64_t    r1:2;         /* reserved */
      uint64_t    vmxe:1;       /* vmx enable */
      uint64_t    smxe:1;       /* smx enable */
      uint64_t    r2:2;         /* reserved */
      uint64_t    pcide:1;      /* process context id */
      uint64_t    osxsave:1;    /* xsave enable */
      uint64_t    r3:1;         /* reserved */
      uint64_t    smep:1;       /* smep */
      uint64_t    r4:11;        /* smep reserved */

   } __attribute__((packed));

   raw64_t;

} __attribute__((packed)) cr4_reg_t;
#define get_cr(_n_)     ({ulong_t x;asm volatile("mov %%cr" #_n_ ", %0":"=r"(x));x;})
#define get_cr0()       get_cr(0)
#define get_cr2()       get_cr(2)
#define get_cr3()       get_cr(3)
#define get_cr4()       get_cr(4)

#define set_cr(_n_,_x_) asm volatile("mov %0, %%cr" #_n_ ::"r"(_x_))
#define set_cr0(x)      set_cr(0,x)
#define set_cr2(x)      set_cr(2,x)
#define set_cr3(x)      set_cr(3,x)
#define set_cr4(x)      set_cr(4,x)

#define enable_paging() ({ulong_t cr0 = get_cr0(); set_cr0(cr0|CR0_PG);})
#define enable_vme()    ({ulong_t cr4 = get_cr4(); set_cr4(cr4|CR4_VME);})
#define disable_vme()   ({ulong_t cr4 = get_cr4(); set_cr4(cr4&(~CR4_VME));})




/*     */
typedef union page_map_level_4_entry
{
   struct
   {
      uint64_t  p:1;
      uint64_t  rw:1;
      uint64_t  lvl:1;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  acc:1;
      uint64_t  mbz:3;
      uint64_t  avl:3;
      uint64_t  addr:40; /* bit 12 */
      uint64_t  avl2:11;
      uint64_t  nx:1;

   } __attribute__((packed));

   raw64_t;

} __attribute__((packed)) pml4e_t;

typedef union page_directory_pointer_entry
{
   struct
   {
      uint64_t  p:1;
      uint64_t  rw:1;
      uint64_t  lvl:1;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  acc:1;
      uint64_t  mbz:3;
      uint64_t  avl:3;
      uint64_t  addr:40; /* bit 12 */
      uint64_t  avl2:11;
      uint64_t  nx:1;

   } __attribute__((packed));

   struct
   {
      uint64_t  p:1;
      uint64_t  r0:2;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  r1:4;
      uint64_t  ign:3;
      uint64_t  addr:40; /* bit 12 */
      uint64_t  r2:12;

   } __attribute__((packed)) pae; /* specific pae pdpte register */
   struct
   {
      uint64_t  p:1;
      uint64_t  rw:1;
      uint64_t  lvl:1;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  acc:1;
      uint64_t  d:1;
      uint64_t  ps:1;     /* bit 7 */
      uint64_t  g:1;
      uint64_t  avl:3;
      uint64_t  pat:1;
      uint64_t  mbz:17;
      uint64_t  addr:22;  /* bit 30 */
      uint64_t  avl2:11;
      uint64_t  nx:1;

   } __attribute__((packed)) page;

   raw64_t;

} __attribute__((packed)) pdpe_t;

typedef union page_directory_entry_64
{
   struct
   {
      uint64_t  p:1;
      uint64_t  rw:1;
      uint64_t  lvl:1;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  acc:1;
      uint64_t  mbz:3;
      uint64_t  avl:3;
      uint64_t  addr:40;   /* bit 12 */
      uint64_t  avl2:11;
      uint64_t  nx:1;

   } __attribute__((packed));

   struct
   {
      uint64_t  p:1;
      uint64_t  rw:1;
      uint64_t  lvl:1;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  acc:1;
      uint64_t  d:1;
      uint64_t  ps:1;      /* bit 7 */
      uint64_t  g:1;
      uint64_t  avl:3;
      uint64_t  pat:1;
      uint64_t  mbz:8;
      uint64_t  addr:31;   /* bit 21 */
      uint64_t  avl2:11;
      uint64_t  nx:1;

   } __attribute__((packed)) page;
   raw64_t;

} __attribute__((packed)) pde64_t;

typedef union page_table_entry_64
{
   struct
   {
      uint64_t  p:1;
      uint64_t  rw:1;
      uint64_t  lvl:1;
      uint64_t  pwt:1;
      uint64_t  pcd:1;
      uint64_t  acc:1;
      uint64_t  d:1;
      uint64_t  pat:1;
      uint64_t  g:1;
      uint64_t  avl:3;
      uint64_t  addr:40;  /* bit 12 */
      uint64_t  avl2:11;
      uint64_t  nx:1;

   } __attribute__((packed));

   raw64_t;

} __attribute__((packed)) pte64_t;
/*
** IA32_EFER_MSR
*/
#define IA32_EFER_MSR          0xc0000080UL

#define IA32_EFER_LME_BIT       8
#define IA32_EFER_LMA_BIT      10
#define IA32_EFER_NXE_BIT      11

#define IA32_EFER_LME          (1<<IA32_EFER_LME_BIT)
#define IA32_EFER_LMA          (1<<IA32_EFER_LMA_BIT)
#define IA32_EFER_NXE          (1<<IA32_EFER_NXE_BIT)
/*
** Used for MSR reading and writing
**
** In 64 bits mode with REX prefix
** upper dword of rax and rdx is 0
**
** MSR is still stored into eax,edx
*/
typedef union {
   struct
   {
      uint32_t  eax;
      uint32_t  edx;

   } __attribute__((packed));

   uint64_t raw;

} __attribute__((packed)) msr_t;
/*
** Read/Write standard MSR (ecx=index, edx,eax=value)
*/
#define rd_msr32(iNdex,vAl)    asm volatile("rdmsr":"=a"((vAl)):"c"((iNdex)):"edx")
#define wr_msr32(iNdex,vAl)    asm volatile("wrmsr"::"c"((iNdex)),"a"((vAl)))

#define rd_msr64(iNdex,hiGh,loW)                                \
   asm volatile("rdmsr" :"=d"((hiGh)),"=a"((loW)):"c"((iNdex)))

#define wr_msr64(iNdex,hiGh,loW)                                        \
     asm volatile("wrmsr" ::"d"((hiGh)),"a"((loW)),"c"((iNdex)))

#define __rd_msr(eax,ecx,edx)            rd_msr64(ecx,edx,eax)
#define __wr_msr(eax,ecx,edx)            wr_msr64(ecx,edx,eax)

typedef union ia32_efer_msr {
   struct
   {
      uint64_t    sce:1;     /* syscall extensions */
      uint64_t    r0:7;      /* zero */
      uint64_t    lme:1;     /* long mode enable */
      uint64_t    r1:1;      /* zero */
      uint64_t    lma:1;     /* long mode active */
      uint64_t    nxe:1;     /* no-execute-enable */

   } __attribute__((packed));

   msr_t;
} __attribute__((packed)) ia32_efer_msr_t;

#define rd_msr_ia32_efer(val)      rd_msr64(IA32_EFER_MSR,(val).edx,(val).eax)
#define wr_msr_ia32_efer(val)      wr_msr64(IA32_EFER_MSR,(val).edx,(val).eax)

#define set_ia32_efer(_n,_v)                    \
   ({                                           \
      ia32_efer_msr_t efer;                     \
      rd_msr_ia32_efer(efer);                   \
      if(_v) efer.raw |=  (1ULL<<(_n));         \
      else   efer.raw &= ~(1ULL<<(_n));         \
      wr_msr_ia32_efer(efer);                   \
   })

#define lm_enable()           set_ia32_efer(IA32_EFER_LME_BIT,1)
#define lm_disable()          set_ia32_efer(IA32_EFER_LME_BIT,0)
#define lm_active()           ({ia32_efer_msr_t m; rd_msr_ia32_efer(m); m.lma;})

/*
		Paging



*/
#define PG_4K_SHIFT                  12
#define PG_4K_SIZE                   (1UL<<PG_4K_SHIFT)
#define pg_4K_nr(addr)               ((addr)>>PG_4K_SHIFT)

/*
** Large pages
*/
#define PG_PS_BIT       7
#define PG_LPAT_BIT     12

#define PG_PS           (1<<PG_PS_BIT)
#define PG_LPAT         (1<<PG_LPAT_BIT)

/*
** Long mode
*/
#define PG_NX_BIT       63
#define PG_NX           (1UL<<PG_NX_BIT)

/*
** Paging bits accessing
*/
#define PG_P_BIT        0
#define PG_RW_BIT       1
#define PG_LVL_BIT      2
#define PG_PWT_BIT      3
#define PG_PCD_BIT      4
#define PG_ACC_BIT      5
#define PG_DRT_BIT      6
#define PG_PAT_BIT      7
#define PG_GLB_BIT      8

#define PG_P            (1<<PG_P_BIT)
#define PG_RO           0
#define PG_RW           (1<<PG_RW_BIT)
#define PG_KRN          0
#define PG_USR          (1<<PG_LVL_BIT)
#define PG_PWT          (1<<PG_PWT_BIT)
#define PG_PCD          (1<<PG_PCD_BIT)
#define PG_ACC          (1<<PG_ACC_BIT)
#define PG_DRT          (1<<PG_DRT_BIT)
#define PG_PAT          (1<<PG_PAT_BIT)
#define PG_GLB          (1<<PG_GLB_BIT)
/*
** 64 bits paging usefull macros
*/
#define PML4E_PER_PML4               512UL
#define PDPE_PER_PDP                 512UL
#define PDE64_PER_PD                 512UL
#define PTE64_PER_PT                 512UL

#define PG_2M_SHIFT                  21
#define PG_2M_SIZE                   (1UL<<PG_2M_SHIFT)
#define pg_2M_offset(addr)           ((addr)&(PG_2M_SIZE-1))
#define pg_2M_nr(addr)               ((addr)>>PG_2M_SHIFT)
#define pg_2M_addr(bits)             ((bits)<<PG_2M_SHIFT)
#define pg_2M_align(addr)            __align(addr,PG_2M_SIZE)
#define pg_2M_align_next(addr)       __align_next(addr,PG_2M_SIZE)
#define pg_2M_aligned(addr)          __aligned(addr,PG_2M_SIZE)

#define PG_1G_SHIFT                  30
#define PG_1G_SIZE                   (1UL<<PG_1G_SHIFT)
#define pg_1G_offset(addr)           ((addr)&(PG_1G_SIZE-1))
#define pg_1G_nr(addr)               ((addr)>>PG_1G_SHIFT)
#define pg_1G_addr(bits)             ((bits)<<PG_1G_SHIFT)
#define pg_1G_align(addr)            __align(addr,PG_1G_SIZE)
#define pg_1G_align_next(addr)       __align_next(addr,PG_1G_SIZE)
#define pg_1G_aligned(addr)          __aligned(addr,PG_1G_SIZE)

#define PG_512G_SHIFT                39
#define PG_512G_SIZE                 (1UL<<PG_512G_SHIFT)
#define pg_512G_offset(addr)         ((addr)&(PG_512G_SIZE-1))
#define pg_512G_nr(addr)             ((addr)>>PG_512G_SHIFT)
#define pg_512G_addr(bits)           ((bits)<<PG_512G_SHIFT)
#define pg_512G_align(addr)          __align(addr,PG_512G_SIZE)
#define pg_512G_align_next(addr)     __align_next(addr,PG_512G_SIZE)
#define pg_512G_aligned(addr)        __aligned(addr,PG_512G_SIZE)

/*
** convenient
*/
#define PAGE_SIZE                    PG_4K_SIZE
#define PAGE_SHIFT                   PG_4K_SHIFT

#define page_align(addr)             __align(addr,PAGE_SIZE)
#define page_align_next(addr)        __align_next(addr,PAGE_SIZE)
#define page_aligned(addr)           __aligned(addr,PAGE_SIZE)

#define page_nr(addr)                pg_4K_nr((offset_t)addr)
#define page_addr(bits)              pg_4K_addr((offset_t)bits)
/*
** absolute index:
**  - pml4_abs_idx == pdp table number
**  - pdp_abs_idx  == pd  table number
**  - pd_abs_idx   == pt  table number
*/
#define pg_abs_idx(addr,shift)       ((addr)>>(shift))
#define pdp_nr(addr)                 ((addr)>>PG_512G_SHIFT)
#define pd64_nr(addr)                ((addr)>>PG_1G_SHIFT)
#define pt64_nr(addr)                ((addr)>>PG_2M_SHIFT)

#define PML4_SZ                      (PML4E_PER_PML4*sizeof(pml4e_t))
#define PDP_SZ                       (PDPE_PER_PDP*sizeof(pdpe_t))
#define PD64_SZ                      (PDE64_PER_PD*sizeof(pde64_t))
#define PT64_SZ                      (PTE64_PER_PT*sizeof(pte64_t))

/*
** Table access
*/
typedef pml4e_t (pml4_t)[PML4E_PER_PML4];
typedef pdpe_t  (pdp_t)[PDPE_PER_PDP];
typedef pde64_t (pd64_t)[PDE64_PER_PD];
typedef pte64_t (pt64_t)[PTE64_PER_PT];

#define pg_set_entry(_e_,_attr_,_pfn_)          \
   ({                                           \
      (_e_)->raw  = (_attr_)|PG_P;              \
      (_e_)->addr = _pfn_;                      \
   })

#define pg_set_large_entry(_e_,_attr_,_pfn_)    \
   ({                                           \
      (_e_)->raw       = (_attr_)|PG_PS|PG_P;   \
      (_e_)->page.addr = _pfn_;                 \
   })


#endif
