PAGE_OFFSET 0x80000000

section .text
enable_paging:
	push ebp
	mov esp, ebp
	mov cr0, eax
	or 0x80000000, eax
	mov eax, cr0
	mov ebp, esp
	pop ebp
	ret

load_page_table:
	push ebp
	mov esp, ebp

	