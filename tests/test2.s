.section .text
    .globl _start

_start:
    # Initialize variables
    li t0, 1        # t0 = 1 (starting number)
    li t1, 10       # t1 = 10 (ending number)
    li t2, 0        # t2 = 0 (sum accumulator)

loop:
    add t2, t2, t0  # t2 = t2 + t0 (add current number to sum)
    addi t0, t0, 1  # t0 = t0 + 1 (increment current number)
    blt t0, t1, loop # If t0 < t1, repeat the loop

    # Exit program
    li a7, 10       # a7 = 10 (exit system call number)
    ecall           # Make system call (exit)