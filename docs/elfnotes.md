### ELF Format
 * ELF Header is 52 bytes for a 32-bit system (risc-v)
 * It Starts at offset 0
 * riscv64-unknown-elf-as -march=rv32imac -o dummy.o dummy.s
 * riscv64-unknown-elf-ld -m elf32lriscv -o dummy.elf dummy.o


00010074 <_start>:
   10074:       42e5                    li      t0,25
   10076:       4345                    li      t1,17
   10078:       006283b3                add     t2,t0,t1
   1007c:       05d00893                li      a7,93
   10080:       0c800513                li      a0,200
   10084:       00000073                ecall



   00010094 <_start>:
   10094:       00001497                auipc   s1,0x1
   10098:       01048493                addi    s1,s1,16 # 110a4 <__DATA_BEGIN__>
   1009c:       4488                    lw      a0,8(s1)
   1009e:       85aa                    mv      a1,a0
   100a0:       00000073                ecall