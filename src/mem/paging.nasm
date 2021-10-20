%define PAGE_OFFSET 0x80000001

global enable_paging
section .text
enable_paging:
	push rbp
	mov rsp, rbp
	mov cr0, rax
	or rax, PAGE_OFFSET
	mov rax, cr0
	mov rbp, rsp
	pop rbp
	ret

load_page_table:
	push rbp
	mov esp, ebp

	