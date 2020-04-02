;[BITS 32]
;
;section .text
;
;loader_entry_point32:
;	push eax
;	push edi
;	push esi
;	push esp
;	push edx
;	push ecx
;
;	; We save pie offset
;    mov ebx, loader_entry_point32
;    sub ebx, [info_offset]
;
;    ; sys_write
;	mov edx, msg_len
;	mov ecx, msg
;	mov ebx, 1
;    mov eax, 4
;    int 0x80
;
;	jmp	start_unpacking
;
;msg	db	"[Unpacking...]", 10, 0
;msg_len	equ	$ - msg
;
;start_unpacking:
;	mov	eax, [info_addr]
;	mov	ecx, [info_size]
;	mov	edx, [info_key]
;
;    ; We add PIE offset
;	add eax, ebx
;
;	add	ecx, eax
;
;.loop:
;	xor	byte [eax], dl
;	ror	edx, 8
;	inc	eax
;	cmp	eax, ecx
;	jnz	.loop
;
;	pop eax
;	pop edi
;	pop esi
;	pop esp
;	pop edx
;	pop ecx
;
;	jmp	0xFFFFFFF
;
;; Random values here, to be patched
;info_key:	    dq	0xEEEEEEEE
;info_addr:	    dq	0xAAAAAAAA
;info_size:	    dq  0xBBBBBBBB
;info_offset:    dq  0xCCCCCCCC

section .text

test_function:
    ; sys_write
	mov edx, msg_len
	mov ecx, msg
	mov ebx, 1
    mov eax, 4
    int 0x80

msg	db	"[Unpacking...]", 10, 0
msg_len	equ	$ - msg

get_eip: mov ebx, [esp]
         ret


start_unpacking:
	mov	eax, [info_addr]
	mov	ecx, [info_size]
	mov	edx, [info_key]

    ; We add PIE offset
	add eax, ebx

	add	ecx, eax

.loop:
	xor	byte [eax], dl
	ror	edx, 8
	inc	eax
	cmp	eax, ecx
	jnz	.loop

	pop eax
	pop edi
	pop esi
	pop esp
	pop edx
	pop ecx

	jmp	0xFFFFFFF

; Random values here, to be patched
info_key:	    dq	0xEEEEEEEE
info_addr:	    dq	0xAAAAAAAA
info_size:	    dq  0xBBBBBBBB
info_offset:    dq  0xCCCCCCCC
