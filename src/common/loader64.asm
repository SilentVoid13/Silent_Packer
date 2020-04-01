
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

global	loader_entry_point64:function
global	loader_size64:data
global	infos_size64:data

loader_size64	dq	end - loader_entry_point64
infos_size64    dq	end - info_start

loader_entry_point64:

	pushfq
	pushx	rax, rdi, rsi, rsp, rdx, rcx

    ; We save pie offset
    lea r12, [rel loader_entry_point64]
    sub r12, [rel info_offset]

    ; sys_write
    mov rax, 1
    ; register order : rdi, rsi, rdx, rcx, r8, r9
	mov	rdi, rax
	lea	rsi, [rel msg]
	mov	rdx, msg_len
	syscall

	jmp	start_unpacking

msg	db	"[Unpacking...]", 10, 0
msg_len	equ	$ - msg

start_unpacking:
	mov	rax, [rel info_addr]
	mov	rcx, [rel info_size]
	mov	rdx, [rel info_key]

    ; We add PIE offset
	add rax, r12

	add	rcx, rax

.loop:
	xor	byte [rax], dl
	ror	rdx, 8
	inc	rax
	cmp	rax, rcx
	jnz	.loop

	popx	rax, rdi, rsi, rsp, rdx, rcx
	popfq
	jmp	0xFFFFFFFF

; Random values here, to be patched
info_start:
info_key:	    dq	0xEEEEEEEEEEEEEEEE
info_addr:	    dq	0xAAAAAAAAAAAAAAAA
info_size:	    dq  0xBBBBBBBBBBBBBBBB
info_offset:    dq  0xCCCCCCCCCCCCCCCC

end: