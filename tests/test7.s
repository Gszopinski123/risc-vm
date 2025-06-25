# test_slt_ecall.S
.global _start
_start:
    addi x10, x0, 5       # x10 => 5
    addi x11, x0, 10      # x11 => 10
    addi x12, x0, -1      # -1 (signed) is 0xFFFFFFFF (unsigned)
    addi x13, x0, 1       # x13 => 1

    slt x14, x10, x11     # x14 = (5 < 10) ? 1 : 0 = 1
    slt x15, x11, x10     # x15 = (10 < 5) ? 1 : 0 = 0
    slt x16, x10, x10     # x16 = (5 < 5) ? 1 : 0 = 0
    slt x17, x12, x13     # x17 = (-1 < 1) ? 1 : 0 = 1 (signed comparison) // failed

    sltu x18, x10, x11    # x18 = (5 < 10) ? 1 : 0 = 1 (unsigned comparison)
    sltu x19, x11, x10    # x19 = (10 < 5) ? 1 : 0 = 0 (unsigned comparison)
    sltu x20, x12, x13    # x20 = (0xFFFFFFFF < 1) ? 1 : 0 = 0 (unsigned comparison, -1 is large)

    ecall
    