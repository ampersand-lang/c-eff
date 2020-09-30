        ;; sets the program to x86_64 mode (instead of the default x86)
        bits 64
        ;; export the `yield` symbol
        global yield
        ;; the yield function
        ;; void yield(void *rdi)
        ;; the `rdi` argument, i.e. the pointer to which we want to yield,
        ;;  gets stored in the rdi register
yield:
.L1:    ; this is just the label we loop to
        ;; the following two instructions are how the frame pointer (rbp) gets restored
        mov rsp, rbp
        pop rbp
        ;; now [rsp] contains the address of the first instruction after the callee
        mov rbx, [rsp]
        ;; we compare that to the argument we got in this function and...
        cmp rbx, rdi
        ;; if the adress is not equal to the adress we want to yield to, we loop
        jne .L1
        ;; otherwise we move that pointer to the instruction pointer, i.e we return
        ret
