# test_arith_ecall.S
.global _start
_start:
    # Initialize registers
    addi x10, x0, 10      # x10 = 10
    addi x11, x0, 5       # x11 = 5

    # ADD
    add x12, x10, x11     # x12 = 10 + 5 = 15

    # SUB
    sub x13, x10, x11     # x13 = 10 - 5 = 5

    # Test with zero
    add x14, x10, x0      # x14 = 10 + 0 = 10
    sub x15, x0, x10      # x15 = 0 - 10 = -10 (0xFFFFFFF6)

    ecall                 # Terminate simulation
