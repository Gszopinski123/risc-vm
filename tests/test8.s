.section .text
.globl _start

_start:
    # Test 1: BEQ (should branch)
    li t0, 5 #pass
    li t1, 5 #pass
    beq t0, t1, label_beq # pass
    beq x0, x0, fail      # if it doesn't branch, fail

label_beq:
    # Test 2: BNE (should branch)
    li t0, 5 #pass
    li t1, 6 #pass
    bne t0, t1, label_bne #pass
    beq x0, x0, fail

label_bne:
    # Test 3: BLT (should branch)
    li t0, 3 #pass
    li t1, 9 #pass
    blt t0, t1, label_blt #pass
    beq x0, x0, fail

label_blt:
    # Successful execution, exit(0)
    li a7, 93     # syscall number for exit
    li a0, 0      # return code
    ecall

fail:
    li a7, 93
    li a0, 1      # return code for failure
    ecall
