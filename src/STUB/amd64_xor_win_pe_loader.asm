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

loader_entry_point:
	pushfq
	pushx rax, rdi, rsi, rsp, rdx, rcx

    ; syscall on windows seems like a pain
    ;mov rax, 1
	;mov	rdi, rax
	;lea	rsi, [rel msg]
	;mov	rdx, msg_len
	;syscall

    ; We save pie offset
    lea r12, [rel loader_entry_point]
    sub r12, [rel info_offset]

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

	popx rax, rdi, rsi, rsp, rdx, rcx
	popfq
	jmp	0xFFFFFFFF

; random values here, to be patched
info_start:
info_key:	    dq	0xaaaaaaaaaaaaaaaa
info_addr:	    dq	0xbbbbbbbbbbbbbbbb
info_size:	    dq  0xcccccccccccccccc
info_offset:    dq  0xdddddddddddddddd
