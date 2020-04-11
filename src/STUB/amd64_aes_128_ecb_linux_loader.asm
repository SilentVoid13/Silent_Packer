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

%macro aes_128_keygenassist 1-2
    aeskeygenassist xmm2, xmm1, %1
    call key_expansion_128
    movdqu %2, xmm1
%endmacro

[BITS 64]

; Linux AMD64 calling convention
%define arg_0 rdi
%define arg_1 rsi
%define arg_2 rdx
%define arg_3 rcx


aes_128_ecb_decrypt:
	pushfq
	pushx rax, rdi, rsi, rsp, rdx, rcx

    ; sys_write
    mov rax, 1
	mov	rdi, rax
	lea	rsi, [rel msg]
	mov	rdx, msg_len
	syscall

    ; We save pie offset
    lea r12, [rel aes_128_ecb_decrypt]
    sub r12, [rel info_offset]

    jmp start_unpacking

key_expansion_128:
    pshufd xmm2, xmm2, 0xff
    vpslldq xmm3, xmm1, 0x4
    pxor xmm1, xmm3
    vpslldq xmm3, xmm1, 0x4
    pxor xmm1, xmm3
    vpslldq xmm3, xmm1, 0x4
    pxor xmm1, xmm3
    pxor xmm1, xmm2
    ret

msg	db	"[Unpacking...]", 10, 0
msg_len	equ	$ - msg

start_unpacking:
    movdqu xmm1, [rel info_key]

    ; Load .text address in rax
    mov rax, [rel info_addr]
    add rax, r12

    ; For later
    mov rdx, [rel info_size]
    ; Move the key in xmm0 to backup
    movdqu xmm0, xmm1

    ; Generating round keys

    ; 10 times
    aes_128_keygenassist 0x1, xmm4
    aes_128_keygenassist 0x2, xmm5
    aes_128_keygenassist 0x4, xmm6
    aes_128_keygenassist 0x8, xmm7
    aes_128_keygenassist 0x10, xmm8
    aes_128_keygenassist 0x20, xmm9
    aes_128_keygenassist 0x40, xmm10
    aes_128_keygenassist 0x80, xmm11
    aes_128_keygenassist 0x1b, xmm12
    aes_128_keygenassist 0x36, xmm13

    ;  Getting ready to use with the Equivalent Inverse Cipher
    ; 9 times - Not on the last one
    aesimc xmm4, xmm4
    aesimc xmm5, xmm5
    aesimc xmm6, xmm6
    aesimc xmm7, xmm7
    aesimc xmm8, xmm8
    aesimc xmm9, xmm9
    aesimc xmm10, xmm10
    aesimc xmm11, xmm11
    aesimc xmm12, xmm12

	xor r8, r8

; Because we can't fully encrypt the .text data when it's not a 16bytes multiple - TOFIX
    sub rdx, 16
aes_loop:
    ; Load .text encrypted section block in xmm15
    movdqu xmm15, [rax + r8]

    pxor xmm15, xmm13 ; First xor
    aesdec xmm15, xmm12 ; Round 1
    aesdec xmm15, xmm11 ; Round 2
    aesdec xmm15, xmm10 ; Round 3
    aesdec xmm15, xmm9 ; Round 4
    aesdec xmm15, xmm8 ; Round 5
    aesdec xmm15, xmm7 ; Round 6
    aesdec xmm15, xmm6 ; Round 7
    aesdec xmm15, xmm5 ; Round 8
    aesdec xmm15, xmm4 ; Round 9
    aesdeclast xmm15, xmm0 ; Round 10

    ; Load the encrypted .text section block
    movdqu [rax + r8], xmm15

    add r8, 16
    cmp rdx, r8
    jle clean
    jmp aes_loop

clean:
    popx rax, rdi, rsi, rsp, rdx, rcx
    popfq
    jmp	0xFFFFFFFF

; random values here, to be patched
info_start:
info_key:	    dq	0xaaaaaaaaaaaaaaaa
info_key1:      dq  0xbbbbbbbbbbbbbbbb
info_addr:	    dq	0xcccccccccccccccc
info_size:	    dq  0xdddddddddddddddd
info_offset:    dq  0xeeeeeeeeeeeeeeee

