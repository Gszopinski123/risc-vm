# test_immediate_arith_ecall.S
.global _start
_start:
    addi x10, x0, 20      # x10 = 20
    addi x11, x0, -5      # x11 = -5 (0xFFFFFFFB)

    addi x12, x10, 10     # x12 = 20 + 10 = 30
    addi x13, x10, -10    # x13 = 20 - 10 = 10

    xori x14, x10, 0xF    # x14 = 20 (0x14) XOR 0xF = 0x1B
    ori  x15, x10, 0xF    # x15 = 20 (0x14) OR 0xF = 0x1F
    andi x16, x10, 0xF    # x16 = 20 (0x14) AND 0xF = 0x4

    # Test with negative immediate
    addi x17, x10, -1     # x17 = 20 - 1 = 19

    ecall
