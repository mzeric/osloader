.text
.global entry
entry:
	cli
	mov $_c_start, %ebp /* setup c stack */
	mov %ebp, %esp
	pushl $0  /* clear flags */
	popf
	movl %ebx, %eax /* multiboot_into_t */
	jmp  c_main
	hlt /* should never be here */
