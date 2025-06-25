# test_shifts_ecall.S
.global _start
_start:
    addi x10, x0, 0x1          # x10 = 1
    addi x11, x0, 4            # x11 = 4 (shift amount)
    addi x12, x0, -8           # x12 = -8 (0xFFFFFFF8)

    sll x13, x10, x11          # x13 = 1 << 4 = 16 (0x10)
    srl x14, x13, x11          # x14 = 16 >> 4 = 1

    sra x15, x12, x11          # x15 = -8 >> 4 (arithmetic right shift) = -1 (0xFFFFFFFF)

    ecall
