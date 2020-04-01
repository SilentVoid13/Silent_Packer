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

[BITS 32]

section .text

global	loader_entry_point32:function
global	loader_size32:data
global	infos_size32:data

loader_size32	dq	end - loader_entry_point32
infos_size32	dq	end - info_start

loader_entry_point32:
	push eax
	push edi
	push esi
	push esp
	push edx
	push ecx

    ; We save pie offset
    mov ebx, loader_entry_point32
    sub edx, info_offset

    ; sys_write
    mov eax, 1
	push msg_len
	push eax
	mov esi, msg
	push esi

	syscall

	jmp	start_unpacking

msg	db	"[Unpacking...]", 10, 0
msg_len	equ	$ - msg

start_unpacking:

	mov	eax, info_addr
	mov	ecx, info_size
	mov	edx, info_key

    ; We add PIE offset
	add eax,ebx

	add	ecx, eax

.loop:
	xor	byte [eax], dl
	ror	edx, 8
	inc	eax
	cmp	eax, ecx
	jnz	.loop

	popx	eax, edi, esi, esp, edx, ecx
	jmp	0xFFFFFFFF

; Random values here, to be patched
info_start:
info_key:	    dq	0xEEEEEEEE
info_addr:	    dq	0xAAAAAAAA
info_size:	    dq  0xBBBBBBBB
info_offset:    dq  0xCCCCCCCC

end:

