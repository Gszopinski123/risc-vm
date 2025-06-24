    .section .data            # Section for data segment
num1:   .word  5              # First number (5)
num2:   .word  10             # Second number (10)

    .section .text            # Section for code segment
    .globl _start             # Make _start the entry point for the program

_start:
    # Load the values of num1 and num2 into registers
    la      t0, num1          # Load address of num1 into t0
    lw      t1, 0(t0)         # Load the value from num1 into t1

    la      t0, num2          # Load address of num2 into t0
    lw      t2, 0(t0)         # Load the value from num2 into t2

    # Perform addition: t1 + t2
    add     t3, t1, t2        # t3 = t1 + t2

    # (Optional) Exit the program
    li      a7, 10            # Load the exit system call code (10) into a7
    ecall                     # Make the system call to exit