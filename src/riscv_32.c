#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
int read_image(const char* image_path);
void read_image_file(FILE * file);
void handle_interrupt(int signal);
uint32_t mem_read(uint32_t address);
void handle_interrupt(int signal);
void checkRegister(int reg);
uint32_t bit_extend(uint32_t val, int bitcnt);
// RISC-V is Little-Endian
// Memory Size for a 32 bit machine
#define MEMORY_MAX ((uint64_t)1 << 30)
// we are going down to 1GB. 4GB was a little too big for my laptop.
uint8_t memory[MEMORY_MAX];
/*
 * I think I am going to implement the easiest type which I believe is R-type
 * I am a pretty Intermediate Programmer so I do not want to get overwhelmed.
 * If this ends up being a little easier than I thought I will continue on
 * I still need to build my own assembler(practice manipulating strings and parsing)
 *
*/
/* Note on Immediates
 * Hard coded numbers will have their sign in the MSB (need to sign-extend to 32)
 *
 */
// Registers
/* Need to make sure the pc and x0 is never changed when the assembly is ran through the vm
 *
 * */
enum 
{
    x0 = 0,
    x1, x2,
    x3, x4, x5,
    x6, x7, x8,
    x9, x10, x11,
    x12, x13, x14,
    x15, x16, x17,
    x18, x19, x20,
    x21, x22, x23,
    x24, x25, x26,
    x27, x28, x29,
    x30, x31, pc, xCOUNT
};
// Register array
uint32_t reg[xCOUNT];

//opcodes are 7 bits
enum {
    r_type = 0x33,
    i_type = 0x13
};
//funct3
enum 
{
    arith = 0x0,//needs funct7
    xor = 0x4,
    or = 0x6,
    and = 0x7,
    sll = 0x1,
    sr = 0x5,//needs funct7
    slt = 0x2,// User determines if the values are signed or not
    sltu = 0x3,
    addi = 0x0,
    xori = 0x4,
    ori = 0x6,
    andi = 0x7,
    slli = 0x1,
    sri = 0x5,
    slti = 0x2,
    sltiu = 0x3
};
enum
{
    add = 0x00,
    sub = 0x20,
    srl = 0x00,
    sra = 0x20,
    srli = 0x00,
    srai = 0x20
};




int main(int argc, char* argv[]) {
    if (argc < 2) {
        printf("No Image!\n");
        exit(2);
    }
    for (int j = 1; j != argc; j++) {
        if (!read_image(argv[j])) {
            printf("Failed to load Image!\n");
        }
    }
    signal(SIGINT, handle_interrupt);   
    enum { PC_START = 0xc000000};
    reg[pc] = PC_START;
    int running = 1;
    int count = 0;
    while (running && count != 19) {
        uint32_t instruction = mem_read(reg[pc]);
        reg[pc] += 4;
        uint8_t opcode = (instruction & 0x7F);
        printf("Instruction: 0x%x\n",instruction);
        printf("Opcode: 0x%x\n",opcode);
        switch (opcode) {
            case r_type:
                printf("R_type\n");
                uint8_t funct3 = ((instruction >> 12) & 0x7);
                uint32_t r1 = ((instruction >> 15) & 0x1f);
                uint32_t r2 = ((instruction >> 15) & 0x1f);
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
                        reg[rd] = (reg[r1] < reg[r2] ? 0x00000001 : 0x00000000);
                        printf("after slt: rd: %d\n",reg[rd]);
                        break;
                    }
                    case sltu://need to fix this technically
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
                        printf("after addi: %d\n",reg[rd]);
                        break;
                    }
                    case xori:
                    {
                        printf("xori\n");
                        reg[rd] = reg[r1] ^ imm;
                        printf("after xori: %d\n",reg[rd]);
                        break;
                    }
                    case ori:
                    {
                        printf("ori\n");
                        reg[rd] = reg[r1] | imm;
                        printf("after xori: %d\n",reg[rd]);
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
                        uint32_t funct7 = imm & 0x3f00;
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
                        reg[rd] = (reg[r1] < bit_extend(imm,12) ? 0x00000001 : 0x00000000);
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
                

            }
                break;
            default:
            {
                printf("No Matching Opcode Yet!\n");
                abort();
                break;
            }
        }
        count++;
    }

    return 0;
}
void handle_interrupt(int signal) {
    printf("Exiting...\n");
    exit(2);
}
void read_image_file(FILE * file) {
    uint32_t origin;// where to place instructions in memory
    fread(&origin,sizeof(uint32_t),1,file);//read it in
    printf("%x\n",origin);
    uint32_t max_read = MEMORY_MAX - origin;
    uint8_t *p = memory + origin;
    size_t bytes = fread(p,sizeof(uint8_t),max_read,file);
    printf("bytes: %zu\n",bytes);
}
int read_image(const char* image_path) {
    FILE *file = fopen(image_path, "rb");
    if (!file) {return 0;}
    read_image_file(file);
    fclose(file);
    return 1;
}
//I am storing big-endian in memory I need to flip this here.
uint32_t mem_read(uint32_t address) {
    printf("0x%x\n",address);
    printf("1st byte: 0x%x\n",memory[address]);
    uint32_t instruction = (memory[address++] & 0xff);
    printf("2nd byte: 0x%x\n",memory[address]);
    instruction |= ((memory[address++] & 0xff) << 8);
    printf("3rd byte: 0x%x\n",memory[address]);
    instruction |= ((memory[address++] & 0xff) << 16);
    printf("4th byte: 0x%x\n",memory[address]);
    instruction |= ((memory[address] & 0xff) << 24);
    return instruction;
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
