.section .text
    .global _start

_start:
    # Using a regular instruction (32-bit)
    li      t0, 5             # Load immediate value 5 into register t0

    # Using a compressed instruction (16-bit)

    # Another regular instruction (32-bit)
    add     t2, t0, t1        # Add t0 and t1, store result in t2

    # Another compressed instruction (16-bit)


    # Exit the program (system call)
    li      a7, 93            # System call number for exit (93 is the syscall number for exit in RISC-V)
    ecall                     # Make the system call to exit
    