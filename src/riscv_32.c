
#include "riscv_32.h"


Elf32_Shdr *shdr;
Elf32_Ehdr elfhdr;
Elf32_Phdr *phdr;
uint8_t memory[MEMORY_MAX];
uint32_t reg[xCOUNT];

int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No Image!\n");
        exit(2);
    }
    for (int j = 1; j != argc; j++) {
        if (!read_objfile(fopen(argv[j],"rb"))) {
            printf("Failed to load Image\n");
        }
    }
    signal(SIGINT, handle_interrupt);   
    enum { PC_START = 0xc000000};
    reg[pc] = elfhdr.e_entry;
    printf("Entry: 0x%x\n",elfhdr.e_entry);
    int running = 1;
    while (running) {
        uint32_t instruction = mem_read(reg[pc]);
        uint8_t opcode = (instruction & 0x7F);
        uint8_t compressed = (instruction & 0x3);
        printf("Instruction: 0x%x\n",instruction);
        // printf("Opcode: 0x%x\n",opcode);
        // printf("count: %d\n",count);
        if (compressed != 0x3) {
            printf("compressed!\n");
            switch (compressed) {
                case ci_css_cr:
                {
                    printf("ci_css_cr\n");
                    uint8_t funct3 = ((instruction >> 13) & 0x7);
                    uint8_t funct4 = ((instruction >> 12) & 0xf);
                    uint8_t ci_rd = ((instruction >> 7) & 0x1f) + 8;
                    switch(funct3) {
                        case clwsp:
                        {
                            printf("clwsp\n");
                            uint32_t ci_imm = (((instruction >> 2) & 0x3) << 6) | (((instruction >> 4) & 0x7) << 2) | (((instruction >> 12) & 0x1) << 5);
                            reg[ci_rd] = 0;
                            reg[ci_rd] |= memory[(reg[x2]+(4*ci_imm))];
                            reg[ci_rd] |= (memory[(reg[x2]+(4*ci_imm))+1] << 8);
                            reg[ci_rd] |= (memory[(reg[x2]+(4*ci_imm))+2] << 16);
                            reg[ci_rd] |= (memory[(reg[x2]+(4*ci_imm))+3] << 24);
                            break;
                        }
                        case cslli:
                        {
                            printf("cslli\n");
                            uint32_t ci_imm = ((((instruction >> 2) & 0x1f) | (((instruction >> 12) & 0x1) << 5)));
                            reg[ci_rd] = reg[ci_rd] << ci_imm;
                            break;
                        }
                        case cswsp:
                        {
                            printf("cswsp\n");
                            uint32_t ci_imm = ((((instruction >> 2) & 0x1f) | (((instruction >> 12) & 0x1) << 5)));
                            uint8_t rs2 = ((instruction >> 2) & 0x1f) + 8;
                            memory[(4*ci_imm)+reg[x2]] = (reg[rs2] & 0xff);
                            memory[(4*ci_imm)+reg[x2]+1] = ((reg[rs2] >> 8) & 0xff);
                            memory[(4*ci_imm)+reg[x2]+2] = ((reg[rs2] >> 16) & 0xff);
                            memory[(4*ci_imm)+reg[x2]+3] = ((reg[rs2] >> 24) & 0xff);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    switch(funct4) {
                        case cjr:// check if register is the null register (zero)
                        {
                            uint8_t rs1 = ((instruction >> 7) & 0x1f);
                            uint8_t rs2 = ((instruction >> 2) & 0x1f);
                            if (rs2 == 0) {//cjr
                                printf("cjr\n");
                                reg[pc] = reg[rs1];
                            } else {// cmv
                                printf("cmv\n");
                                reg[rs1] = reg[rs2];// Implicit + 0 at the end
                                printf("%d\n",reg[rs2]);
                            }
                            break;
                        }
                        case cjalr: // adding and ebreak
                        {
                            printf("cjalr, cadd, cebreak\n");
                            uint8_t rs1_rd = ((instruction >> 7) & 0x1f) + 8;
                            uint8_t rs2 = ((instruction >> 2) & 0x1f) + 8;
                            if (rs2 == 0 && rs1_rd == 0) {//Ecall
                                printf("cecall\n");
                                // RAISE TRAPS but for now we will just end the program
                                running = 0;
                            } else if (rs2 == 0 && rs1_rd != 0) {
                                printf("cjalr\n");
                                reg[x1] = reg[pc] + 2;
                                reg[pc] = reg[rs1_rd];
                            } else {
                                printf("cadd\n");
                                reg[rs1_rd] += reg[rs2];
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
                case cl_ciw:
                {
                    printf("cl_ciw\n");
                    uint8_t funct3 = ((instruction >> 13) & 0x7);
                    switch(funct3) {
                        case caddi4spn:
                        {
                            printf("caddi4spn");
                            uint32_t imm = (((instruction >> 5) & 0x1) << 3) | (((instruction >> 6) & 0x1) << 2) | 
                            (((instruction >> 7) & 0xf) << 6) | (((instruction >> 11) & 0x3) << 4);
                            uint8_t rd = ((instruction >> 2) & 0x7) + 8;
                            reg[rd] = x2 + (imm * 4);
                            break;
                        }
                        case clw:
                        {
                            printf("clw\n");
                            uint8_t rd = ((instruction >> 2) & 0x7) + 8;
                            printf("reg[rd]: 0x%x\n",reg[rd]);
                            uint8_t rs1 = ((instruction >> 7) & 0x7) + 8;
                            printf("reg[rs1]: 0x%x\n",reg[rs1]);
                            uint32_t imm = (((instruction >> 5) & 0x1) << 5) | (((instruction >> 6) & 0x1) << 2) | (((instruction >> 10) & 0x7) << 3);
                            printf("imm: 0x%x\n",imm);
                            reg[rd] = 0;
                            reg[rd] |= memory[((imm) + reg[rs1])];
                            reg[rd] |= (memory[((imm) + reg[rs1])+1] << 8);
                            reg[rd] |= (memory[((imm) + reg[rs1])+2] << 16);
                            reg[rd] |= (memory[((imm) + reg[rs1])+3] << 24);
                            printf("after clw: 0x%x\n",reg[rd]);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
                case ci_cj_cb_cs:
                {
                    printf("ci_cj_cb\n");
                    uint8_t funct3 = ((instruction >> 13) & 0x7);
                    switch(funct3) {
                        case cli:
                        {
                            printf("cli\n");
                            uint8_t rd = ((instruction >> 7) & 0x1f);
                            uint32_t imm = ((instruction >> 2) & 0x1f) | (((instruction >> 12) & 0x1) << 5);
                            reg[rd] = bit_extend(imm,6);
                            printf("reg[rd]: 0x%x\n",reg[rd]);
                            break;
                        }
                        case clui://caddi16sp has the same label
                        {
                            printf("clui, caddi16sp\n");
                            uint8_t rd = ((instruction >> 7) & 0x1f);
                            if (rd == 2) {
                                uint32_t imm = bit_extend((((instruction >> 2) & 0x1) << 5) |
                                               (((instruction >> 3) & 0x3) << 7) |
                                               (((instruction >> 5) & 0x1) << 6) |
                                               (((instruction >> 6) & 0x1) << 4) |
                                               (((instruction >> 12) & 0x1) << 9),7);
                                reg[rd] = reg[rd] + 4*imm;
                            } else {
                                uint32_t imm = ((((instruction >> 2) & 0x1f) | (((instruction >> 12) & 0x1) << 5)) << 12);
                                reg[rd] = bit_extend(imm,6);
                            }
                            break;
                        }
                        case caddi:// cnop has the same label
                        {
                            printf("caddi, cnop\n");
                            uint8_t rd = ((instruction >> 7) & 0x1f);
                            if (rd == 0) {
                                printf("cnop\n");
                            } else {
                                printf("caddi\n");
                                uint32_t imm = bit_extend(((instruction >> 2) & 0x1f) | (((instruction >> 12) & 0x1) << 5),7);
                                reg[rd] = imm;
                                printf("reg[rd] = 0x%x\n",reg[rd]);
                            }
                            break;
                        }
                        case cj:
                        {
                            printf("cj\n");
                            uint32_t offset = bit_extend(((((instruction >> 2) & 0x1) << 5) |
                                              (((instruction >> 3) & 0x7) << 1)  |
                                              (((instruction >> 6) & 0x1) << 7)  |
                                              (((instruction >> 7) & 0x1) << 6)  |
                                              (((instruction >> 8) & 0x1) << 10) |
                                              (((instruction >> 9) & 0x3) << 8)  |
                                              (((instruction >> 11) & 0x1) << 4) |
                                              (((instruction >> 12) & 0x1)) << 11), 11);
                            reg[pc] = reg[pc] + offset;
                            break;
                        }
                        case cjal:
                        {
                            printf("cjal\n");
                            uint32_t offset = bit_extend(((((instruction >> 2) & 0x1) << 5) |
                                              (((instruction >> 3) & 0x7) << 1)  |
                                              (((instruction >> 6) & 0x1) << 7)  |
                                              (((instruction >> 7) & 0x1) << 6)  |
                                              (((instruction >> 8) & 0x1) << 10) |
                                              (((instruction >> 9) & 0x3) << 8)  |
                                              (((instruction >> 11) & 0x1) << 4) |
                                              (((instruction >> 12) & 0x1)) << 11), 11);
                            reg[x1] = reg[pc] + 2;
                            reg[pc] = reg[pc] + offset;
                            break;
                        }
                        case cbeqz:
                        {
                            printf("cbeqz\n");
                            uint32_t rs1 = ((instruction >> 7) & 0x3) + 8;
                            uint32_t imm = bit_extend((((instruction >> 2) & 0x1) << 5)  | 
                                           (((instruction >> 3) & 0x3) << 1)  |
                                           (((instruction >> 5) & 0x3) << 6)  |
                                           (((instruction >> 10) & 0x3) << 3) |
                                           (((instruction >> 12) & 0x1) << 8),8);
                            if (!reg[rs1]) {
                                reg[pc] = reg[pc] + imm;
                            }
                            break;
                        }
                        case cbnez:
                        {
                            printf("cbnez\n");
                            uint32_t rs1 = ((instruction >> 7) & 0x3) + 8;
                            uint32_t imm = bit_extend((((instruction >> 2) & 0x1) << 5)  | 
                                           (((instruction >> 3) & 0x3) << 1)  |
                                           (((instruction >> 5) & 0x3) << 6)  |
                                           (((instruction >> 10) & 0x3) << 3) |
                                           (((instruction >> 12) & 0x1) << 8),8);
                            if (reg[rs1]) {
                                reg[pc] = reg[pc] + imm;
                            }
                            break;
                        }
                        case cb_cs: // 0x4
                        {
                            printf("cb_cs\n");
                            uint8_t shift2 = ((instruction >> 10) & 0x3);
                            uint8_t alu8 = (((instruction >> 10) & 0x3f) << 2) | ((instruction >> 5) & 0x3);
                            switch(shift2) {
                                case csrai:
                                {
                                    printf("csrai\n");
                                    uint8_t rd = 8 + ((instruction >> 7) & 0x3);
                                    uint32_t signbit = reg[rd] & 0x80000000;
                                    
                                    uint32_t imm = bit_extend((((instruction >> 12) & 0x1) << 5)
                                                                | ((instruction >> 2) & 0x1f),6);
                                    reg[rd] = (reg[rd] >> imm) | signbit;
                                    printf("after csrai: 0x%x\n",reg[rd]);
                                    break;
                                }
                                case csrli:
                                {
                                    printf("csrli\n");
                                    uint8_t rd = 8 + ((instruction >> 7) & 0x3);
                                    
                                    uint32_t imm = bit_extend((((instruction >> 12) & 0x1) << 5)
                                                                | ((instruction >> 2) & 0x1f),6);
                                    reg[rd] = (reg[rd] >> imm);
                                    printf("after csrli: 0x%x\n",reg[rd]);
                                    break;
                                }
                                case candi:
                                {
                                    
                                    printf("candi\n");
                                    uint8_t rd = 8 + ((instruction >> 7) & 0x3);
                                    
                                    uint32_t imm = bit_extend((((instruction >> 12) & 0x1) << 5)
                                                                | ((instruction >> 2) & 0x1f),6);
                                    reg[rd] = (reg[rd] & imm);
                                    printf("after candi: 0x%x\n",reg[rd]);
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                            }
                            switch(alu8) {
                                case cand:
                                {
                                    printf("cand\n");
                                    uint8_t rd  = 8 + ((instruction & 0x3 >> 7));
                                    uint8_t rs2 = 8 + ((instruction & 0x3) >> 2);
                                    reg[rd] = reg[rd] & reg[rs2];
                                    printf("after cand: 0x%x\n",reg[rd]);
                                    break;
                                }
                                case cor:
                                {
                                    printf("cor\n");
                                    uint8_t rd  = 8 + ((instruction & 0x3 >> 7));
                                    uint8_t rs2 = 8 + ((instruction & 0x3) >> 2);
                                    reg[rd] = reg[rd] | reg[rs2];
                                    printf("after cor: 0x%x\n",reg[rd]);
                                    break;
                                }
                                case cxor:
                                {
                                    printf("cxor\n");
                                    uint8_t rd  = 8 + ((instruction & 0x3 >> 7));
                                    uint8_t rs2 = 8 + ((instruction & 0x3) >> 2);
                                    reg[rd] = reg[rd] ^ reg[rs2];
                                    printf("after cxor: 0x%x\n",reg[rd]);
                                    break;
                                }
                                case csub:
                                {
                                    printf("csub\n");
                                    uint8_t rd  = 8 + ((instruction & 0x3 >> 7));
                                    uint8_t rs2 = 8 + ((instruction & 0x3) >> 2);
                                    reg[rd] = reg[rd] - reg[rs2];
                                    printf("after csub: 0x%x\n",reg[rd]);
                                    break;
                                }
                                default:
                                {
                                    break;
                                }
                            }
                            break;
                        }
                    }   
                    break;
                }
                default:
                    break;
            }
            reg[pc] += 2;//only loaded in a 2 bytes   
        } else {
            switch (opcode) {
                case r_type:
                    printf("R_type\n");
                    uint8_t funct3 = ((instruction >> 12) & 0x7);
                    uint32_t r1 = ((instruction >> 15) & 0x1f);
                    uint32_t r2 = ((instruction >> 20) & 0x1f);
                    uint32_t check = ((instruction >> 7) & 0x1f);
                    checkRegister(check);
                    uint32_t rd = check;
                    printf("r1: %d, r2: %d, rd: %d\n",r1,r2,rd);
                    switch(funct3) {
                        case arith:
                        {
                            printf("Arith\n");
                            uint8_t funct7 = ((instruction >> 25) & 0x7f);
                            switch (funct7) {
                                case add: {
                                    printf("add\n");
                                    reg[rd] = reg[r1] + reg[r2];
                                    printf("After addition; rd: %d\n",reg[rd]);
                                    break;
                                }
                                case sub:
                                {
                                    printf("sub\n");
                                    reg[rd] = reg[r1] - reg[r2];
                                    printf("After Subtraction; rd: %d\n",reg[rd]);
                                    break;
                                }
                                default:
                                {
                                    abort();
                                    break;
                                }
                            }
                            break;
                        }
                        case xor:
                        {
                            printf("xor\n");
                            reg[rd] = reg[r1] ^ reg[r2];
                            printf("After xor; rd: %d\n",reg[rd]);
                            break;
                        }
                        case or:
                        {
                            printf("or\n");
                            reg[rd] = reg[r1] | reg[r2];
                            printf("After or; rd: %d\n",reg[rd]);
                            break;
                        }
                        case and:
                        {
                            printf("and\n");
                            reg[rd] = reg[r1] & reg[r2];
                            printf("After and; rd: %d\n",reg[rd]);
                            break;
                        }
                        case sll:
                        {
                            printf("sll\n");
                            reg[rd] = reg[r1] << reg[r2];
                            printf("After sll; rd: %d\n",reg[rd]);
                            break;
                        }
                        case sr:
                        {
                            printf("sr\n");
                            uint8_t funct7 = ((instruction >> 25) & 0x7f);
                            switch (funct7) {
                                case srl:
                                {
                                    printf("srl\n");
                                    reg[rd] = reg[r1] >> reg[r2];
                                    printf("After srl; rd: %d\n",reg[rd]);
                                    break;
                                }
                                case sra:
                                {
                                    printf("sra\n");
                                    uint32_t signbit = (((reg[r1] >> 31) & 1) << 31);
                                    for (int i = 0; i != reg[r2]; i++)
                                        reg[r1] = ((reg[r1] >> 1) | signbit);
                                    reg[rd] = reg[r1];
                                    printf("After sra; rd: %d\n",reg[rd]);
                                    break;
                                }
                                default:
                                {
                                    abort();
                                    break;
                                }
                            }
                            break;
                        }
                        case slt://this is fine
                        {
                            printf("slt\n");
                            int32_t reg_r2 = reg[r2];
                            int32_t reg_r1 = reg[r1];
                            reg[rd] = (reg_r1 < reg_r2 ? 0x00000001 : 0x00000000);
                            printf("after slt: rd: %d\n",reg[rd]);
                            break;
                        }
                        case sltu:
                        {
                            printf("sltu\n");
                            reg[rd] = (reg[r1] < reg[r2] ? 0x00000001 : 0x00000000);
                            printf("after sltu: rd: %d\n",reg[rd]);
                            break;
                        }
                        default:
                        {
                            abort();
                            break;
                        }
                    }
                    break;
                case i_type:
                {
                    printf("I_type\n");
                    uint8_t funct3 = ((instruction >> 12) & 0x7);
                    uint32_t check = ((instruction >> 7) & 0x1f);
                    uint32_t r1 = ((instruction >> 15) & 0x1f);
                    uint32_t imm = bit_extend(((instruction >> 20) & 0xfff),12);
                    checkRegister(check);
                    uint32_t rd = check;
                    printf("r1: %d, imm: %d, rd: %d\n",r1,imm,rd);
                    switch(funct3) {
                        case addi:
                        {
                            printf("addi\n");
                            reg[rd] = reg[r1] + imm;
                            printf("after addi: 0x%x\n",reg[rd]);
                            break;
                        }
                        case xori:
                        {
                            printf("xori\n");
                            reg[rd] = reg[r1] ^ imm;
                            printf("after xori: 0x%x\n",reg[rd]);
                            break;
                        }
                        case ori:
                        {
                            printf("ori\n");
                            reg[rd] = reg[r1] | imm;
                            printf("after ori: 0x%x\n",reg[rd]);
                            break;
                        }
                        case andi:
                        {
                            printf("andi\n");
                            reg[rd] = reg[r1] & imm;
                            printf("after andi: %d\n",reg[rd]);
                            break;
                        }
                        case slli:
                        {
                            printf("slli\n");
                            reg[rd] = reg[r1] << (imm & 0x1f);
                            printf("after slli: %d\n",reg[rd]);
                            break;
                        }
                        case sri:
                        {
                            printf("sri\n");
                            uint32_t funct7 = imm & 0x3f00;
                            switch(funct7) {
                                case srai:
                                {
                                    printf("srai\n");
                                    uint32_t signbit = (((reg[r1] >> 31) & 1) << 31);
                                    for (int i = 0; i != (imm & 0x1f); i++)
                                        reg[r1] = ((reg[r1] >> 1) | signbit);
                                    reg[rd] = reg[r1];
                                    printf("after srai: %d\n",reg[rd]);
                                    break;
                                }
                                case srli:
                                {
                                    printf("srli\n");
                                    reg[rd] = reg[r1] >> (imm & 0x1f);
                                    printf("after srli: %d\n",reg[rd]);
                                    break;
                                }
                            }
                            break;
                        }
                        case slti:
                        {
                            printf("slti\n");
                            int32_t reg_r1 = reg[r1];
                            reg[rd] = (reg_r1 < bit_extend(imm,12) ? 0x00000001 : 0x00000000);
                            printf("after slti: %d\n",reg[rd]);
                            break;
                        }
                        case sltiu:
                        {
                            printf("sltiu\n");
                            reg[rd] = (reg[r1] < imm ? 0x00000001 : 0x00000000);
                            printf("after sltiu: %d\n",reg[rd]);
                            break;
                        }
                    }
                    break;
                }
                case i2_typ:
                {
                    printf("i2_typ\n");
                    uint8_t funct3 = ((instruction >> 12) & 0x7);
                    uint32_t check = ((instruction >> 7) & 0x1f);
                    uint32_t r1 = ((instruction >> 15) & 0x1f);
                    uint32_t imm = bit_extend(((instruction >> 20) & 0xfff),12);
                    uint32_t rd = check;
                    switch(funct3) {
                        case lb:
                        {
                            printf("lb\n");
                            reg[rd] = 0;
                            reg[rd] = memory[reg[r1]+imm];
                            printf("after lb: %d\n",reg[rd]);
                            break;
                        }
                        case lh:
                        {
                            printf("lh\n");
                            reg[rd] = 0;
                            reg[rd] |= memory[reg[r1]+imm];
                            reg[rd] |= (memory[reg[r1]+imm+1] << 8);
                            printf("after lh: %d\n",reg[rd]);
                            break;
                        }
                        case lw:
                        {
                            printf("lw\n");
                            reg[rd] = 0;
                            reg[rd] |= memory[reg[r1]+imm];
                            reg[rd] |= (memory[reg[r1]+imm+1] << 8);
                            reg[rd] |= (memory[reg[r1]+imm+2] << 16);
                            reg[rd] |= (memory[reg[r1]+imm+3] << 24);
                            printf("after lw: 0x%x\n",reg[rd]);
                            break;
                        }
                        case lbu:
                        {
                            printf("lbu\n");
                            reg[rd] = 0;
                            reg[rd] = memory[reg[r1]+imm];
                            printf("after lbu: %d\n",reg[rd]);
                            break;
                        }
                        case lhu:
                        {
                            printf("lhu\n");
                            reg[rd] = 0;
                            reg[rd] |= memory[reg[r1]+imm];
                            reg[rd] |= (memory[reg[r1]+imm+1] << 8);
                            printf("after lhu: %d\n",reg[rd]);
                            break;
                        }
                    }
                    break;
                }
                case s_type:
                {
                    printf("s_type\n");
                    uint8_t funct3 = ((instruction >> 12) & 0x7);
                    uint32_t imm = bit_extend(((instruction >> 7) & 0x1f) | (((instruction >> 25) & 0x7f) << 5),12);
                    uint8_t rs1 = ((instruction >> 15) & 0x1f);
                    uint8_t rs2 = ((instruction >> 20) & 0x1f);
                    switch(funct3) {
                        case sb:
                        {
                            printf("sb\n");
                            memory[reg[rs1] + imm] = (reg[rs2] & 0xff);
                            break;
                        }
                        case sh:
                        {
                            printf("sh\n");
                            memory[reg[rs1] + imm] = (reg[rs2] & 0xff);
                            memory[reg[rs1] + imm + 1] = ((reg[rs2] >> 8) & 0xff);
                            break;
                        }
                        case sw:
                        {
                            printf("sw\n");
                            printf("reg[rs1]: 0x%x reg[rs2]: 0x%x\n",reg[rs1],reg[rs2]);
                            memory[reg[rs1] + imm] = (reg[rs2] & 0xff);
                            memory[reg[rs1] + imm + 1] = ((reg[rs2] >> 8) & 0xff);
                            memory[reg[rs1] + imm + 2] = ((reg[rs2] >> 16) & 0xff);
                            memory[reg[rs1] + imm + 3] = ((reg[rs2] >> 24) & 0xff);
                            printf("memory: 0x%x%x%x%x\n",memory[reg[rs1] + imm],
                                                          memory[reg[rs1] + imm+1],
                                                          memory[reg[rs1] + imm+2],
                                                          memory[reg[rs1] + imm+3]);
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
                case u_type:
                {
                    printf("u_type\n");
                    printf("lui\n");
                    uint8_t rd = ((instruction >> 7) & 0x1f);
                    uint32_t imm = (((instruction >> 12) & 0xfffff) << 12);
                    reg[rd] = imm;
                    printf("reg[rd]: 0x%x\n",reg[rd]);
                    break;
                }
                case u2_typ:
                {
                    printf("u_typ2\n");
                    printf("auipc\n");
                    uint8_t rd = ((instruction >> 7) & 0x1f);
                    uint32_t imm = (((instruction >> 12) & 0xfffff) << 12);
                    printf("imm: 0x%x\n",imm);
                    reg[rd] = reg[pc] + imm;
                    printf("reg[rd]: 0x%x\n",reg[rd]);
                    break;
                }
                case i3_typ:
                {
                    uint32_t imm = ((instruction >> 20) & 0x1);
                    if (imm == 1) {//ebreak
                        //raise debugger
                    } else {//ecall
                        //raise TRAPS here
                        running = 0;
                    }
                    break;
                }
                case b_type:
                {
                    printf("b_type\n");
                    uint8_t funct3 = ((instruction >> 12) & 0x7);
                    uint8_t rs1 = ((instruction >> 15) & 0x1f);
                    uint8_t rs2 = ((instruction >> 20) & 0x1f);
                    uint32_t imm = bit_extend(((instruction >> 8) & 0xf) | 
                                              (((instruction >> 7) & 0x1) << 11) | 
                                              (((instruction >> 25) & 0x3f) << 5) |
                                              (((instruction >> 31) & 0x1) << 12) ,11);
                    switch(funct3) {
                        case beq:
                        {
                            printf("beq\n");
                            if (reg[rs1] == reg[rs2]) {
                                reg[pc] += imm;
                                printf("move pc!\n");
                            }
                            break;
                        }
                        case bne:
                        {
                            printf("bne\n");
                            if (reg[rs1] != reg[rs2]) {
                                reg[pc] += imm;
                                printf("move pc!\n");
                            }
                            break;
                        }
                        case blt:
                        {
                            printf("blt\n");
                            int32_t reg_rs1 = reg[rs1];
                            int32_t reg_rs2 = reg[rs2];
                            if (reg_rs1 < reg_rs2) {
                                reg[pc] += imm;
                                printf("move pc!\n");
                            }
                            break;
                        }
                        case bge:
                        {
                            printf("bge\n");
                            int32_t reg_rs1 = reg[rs1];
                            int32_t reg_rs2 = reg[rs2];
                            if (reg_rs1 >= reg_rs2) {
                                reg[pc] += imm;
                                printf("move pc!\n");
                            }
                            break;
                        }
                        case bltu:
                        {
                            printf("bltu\n");
                            if (reg[rs1] < reg[rs2]) {
                                reg[pc] += imm;
                                printf("move pc!\n");
                            }
                            break;
                        }
                        case bgeu:
                        {
                            printf("bgeu\n");
                            if (reg[rs1] >= reg[rs2]) {
                                reg[pc] += imm;
                                printf("move pc!\n");
                            }
                            break;
                        }
                        default:
                        {
                            break;
                        }
                    }
                    break;
                }
                case j_type:
                {
                    printf("j_type\n");
                    uint8_t rd = ((instruction >> 7) & 0x1f);
                    int32_t imm = bit_extend((((instruction >> 12) & 0xff) << 12) | 
                                  (((instruction >> 20) & 0x1) << 11)  | 
                                  (((instruction >> 21) & 0x3ff) << 1) |
                                  (((instruction >> 31) & 0x1) << 20) , 20);
                    reg[rd] = reg[pc]+4;
                    reg[pc] += imm;
                    printf("jal\n");
                    break;
                }
                case jalr:
                {
                    printf("jalr\n");
                    uint8_t rd = ((instruction >> 7) & 0x1f);
                    uint8_t rs1 = ((instruction >> 15) & 0x1f);
                    uint32_t imm = bit_extend(((instruction >> 20) & 0xfff),12);
                    reg[rd] = reg[pc] + 4;
                    reg[pc] = reg[rs1] + imm;
                    printf("end of jalr\n");
                    break;
                }
                default:
                {
                    printf("No Matching Opcode Yet!\n");
                    abort();
                    break;
                }
                  
            }
            reg[pc] += 4;
        }
    }

    return 0;
}

int read_objfile(FILE *file) {
    size_t bytes = fread(&elfhdr,sizeof(elfhdr),1,file);
    if (bytes == 0) {
        return 0;
    }
    printf("0x%x\n",elfhdr.e_entry);
    phdr = malloc(sizeof(Elf32_Phdr)*elfhdr.e_phnum);
    if (phdr == NULL) {
        printf("Memory not allocated!\n");
        exit(-1);
    }
    shdr = malloc(sizeof(Elf32_Shdr)*elfhdr.e_shnum);
    if (shdr == NULL) {
        printf("Memory not allocated\n");
        exit(-1);
    }
    fseek(file,elfhdr.e_phoff,SEEK_SET);
    fread(phdr,sizeof(Elf32_Phdr),elfhdr.e_phnum,file);
    for (int i = 0; i != elfhdr.e_phnum; ++i) {
        if (PT_LOAD == phdr[i].p_type) {
            fseek(file,phdr[i].p_offset, SEEK_SET);
            bytes = fread(memory + (phdr[i].p_vaddr), phdr[i].p_memsz, 1, file);
            if (bytes != 1) {
                abort();
            }
        }
    }
    fseek(file,elfhdr.e_shoff,SEEK_SET);
    fread(shdr,sizeof(Elf32_Shdr),elfhdr.e_shnum,file);
    fseek(file,shdr[elfhdr.e_shstrndx].sh_offset,SEEK_SET);
    for (int i = 0; i != elfhdr.e_shnum; ++i) {
        fseek(file,shdr[i].sh_offset, SEEK_SET);
        bytes = fread(memory + (shdr[i].sh_addr), shdr[i].sh_size, 1, file);
    }
    return 1;
}

uint32_t mem_read(uint32_t address) {
    uint32_t instruction = ((memory[address++] & 0xff));
    instruction |= ((memory[address++] & 0xff) << 8);
    if ((instruction & 0x3) != 3) {
        printf("All Compressed!\n");
        return instruction;
    }
    instruction |= ((memory[address++] & 0xff) << 16);
    instruction |= ((memory[address] & 0xff) << 24);
    return instruction;
}
void handle_interrupt(int signal) {
    printf("Exiting...\n");
    exit(2);
}

// Makes sure user is not. Do not want user to mutate null register and pc.
void checkRegister(int reg) {
    if (reg == 0) {
        abort();
    } else if (reg == 32) {
        abort();
    }
}

uint32_t bit_extend(uint32_t val, int bitcnt) {
    uint32_t sign = (val >> (bitcnt - 1));
    uint32_t dummy = 0;
    for (int i = 0; i != bitcnt - 1; ++i) {
        dummy |= (1 << i);
    }
    uint32_t rval;
    if (sign) {
        rval = ~((~val + 1) & dummy)+1;
    } else {
        rval = val;
    }
    return rval;
}