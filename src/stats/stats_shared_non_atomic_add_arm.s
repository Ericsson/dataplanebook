ep_stats_update(unsigned int, unsigned short):                  // @ep_stats_update(unsigned int, unsigned short)
        adrp    x8, stats
        add     x8, x8, :lo12:stats
        ldr     x9, [x8, #16008]
        ldr     x10, [x8, #16000]
        add     x11, x8, w0, uxtw #4
        add     x9, x9, #1                      // =1
        str     x9, [x8, #16008]
        and     x9, x1, #0xffff
        add     x10, x10, x9
        str     x10, [x8, #16000]
        ldp     x10, x8, [x11]
        add     x8, x8, #1                      // =1
        add     x9, x10, x9
        stp     x9, x8, [x11]
        ret
stats:
        .zero   16016
