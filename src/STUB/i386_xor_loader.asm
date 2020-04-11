%macro pushx 1-*
 %rep %0
   push %1
   %rotate 1
 %endrep
%endmacro

%macro popx 1-*
  %rep %0
    %rotate -1
    pop %1
  %endrep
%endmacro

section .text

loader_entry_point32:
	pushx eax, edi, esi, esp, edx, ecx, ebx

    ; syscall : eax
    ; parameters order : ebx, ecx, edx, esi, edi, ebp
    ; sys_write
    call get_my_loc
    sub edx, next_i - msg
	mov ecx, edx
	mov edx, msg_len
	mov ebx, 1
    mov eax, 4
    int 0x80

    ; We save pie offset
    call get_my_loc
    sub edx, next_i - loader_entry_point32
    mov ebx, edx
    call get_my_loc
    sub edx, next_i - info_offset
    sub ebx, [edx]

	jmp	start_unpacking

msg	db	"[Unpacking...]", 10, 0
msg_len	equ	$ - msg

get_my_loc:
    call next_i

next_i:
    pop edx
    ret

start_unpacking:
    call get_my_loc
    sub edx, next_i - info_addr
	mov	eax, [edx]
	call get_my_loc
	sub edx, next_i - info_size
	mov	ecx, [edx]
	call get_my_loc
	sub edx, next_i - info_key
	mov	edx, [edx]

    ; We add PIE offset
	add eax, ebx
	add	ecx, eax

.loop:
	xor	byte [eax], dl
	ror	edx, 4
	inc	eax
	cmp	eax, ecx
	jnz	.loop

	popx eax, edi, esi, esp, edx, ecx, ebx

	jmp	0xFFFFFFF

; Random values here, to be patched
info_key:	    dd	0xaaaaaaaa
info_addr:	    dd	0xbbbbbbbb
info_size:	    dd  0xcccccccc
info_offset:    dd  0xdddddddd
