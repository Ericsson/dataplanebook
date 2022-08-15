ep_stats_update(unsigned int, unsigned short):
        movzx   esi, si
        mov     eax, 1
        vmovq   xmm1, rsi
        vpinsrq xmm0, xmm1, rax, 1
        vpaddq  xmm0, xmm0, XMMWORD PTR stats[rip+16000]
        mov     edi, edi
        sal     rdi, 4
        inc     QWORD PTR stats[rdi+8]
        add     QWORD PTR stats[rdi], rsi
        vmovdqa XMMWORD PTR stats[rip+16000], xmm0
        ret
stats:
        .zero   16016
