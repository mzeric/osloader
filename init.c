#include "types.h"
#include "elf.h"
#include "multiboot.h"

void __attribute__((regparm(1))) c_main(multiboot_info_t *info){
	multiboot_module_t *mod = info->mods_addr;
	i386_cpu_init();
	enter_long_mode(info);
}
