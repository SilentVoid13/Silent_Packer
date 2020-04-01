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

[BITS 64]

section .text

global	loader_entry_point32:function
global	loader_size32:data
global	infos_size32:data

loader_size32	dq	end - loader_entry_point32
infos_size32	dq	end - info_start

loader_entry_point32:
	push rax
	push rdi
	push rsi
	push rsp
	push rdx
	push rcx

    ; We save pie offset
    mov rdx, loader_entry_point32
    sub rdx, info_offset

    ; sys_write
    mov rax, 1
	push msg_len
	push rax
	mov rsi, msg
	push rsi

	syscall

	jmp	start_unpacking

msg	db	"[Unpacking...]", 10, 0
msg_len	equ	$ - msg

start_unpacking:

	mov	rbx, info_addr
	mov	rcx, info_size
	mov	rax, info_key

    ; We add PIE offset
	add rbx,rdx

	add	rcx, rbx

.loop:
	xor	dl, bl
	ror	rax, 8
	inc	rbx
	cmp	rbx, rcx
	jnz	.loop

	popx	rax, rdi, rsi, rsp, rdx, rcx
	jmp	0xFFFFFFFF

; Random values here, to be patched
info_start:
info_key:	    dq	0xEEEEEEEE
info_addr:	    dq	0xAAAAAAAA
info_size:	    dq  0xBBBBBBBB
info_offset:    dq  0xCCCCCCCC

end:

