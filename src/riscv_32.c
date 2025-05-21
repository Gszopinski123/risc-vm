#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#define ELF_HEADER 52
#define PROGRAM_HEADER 32
#define PT_LOAD 1
//FUNCTION PROTOTYPES
int read_image(const char* image_path);
void read_image_file(FILE * file);
void handle_interrupt(int signal);
uint32_t mem_read(uint32_t address);
void handle_interrupt(int signal);
void checkRegister(int reg);
uint32_t bit_extend(uint32_t val, int bitcnt);
void read_objfile(FILE *file);
//ELF Header Structure (52 bytes for a 32-bit System)
typedef struct {
    unsigned char e_ident[16]; //Magic Number and ELF ID
    uint16_t e_type; //object file type
    uint16_t e_machine; //Architecture
    uint32_t e_version; //Object file version
    uint32_t e_entry; //Entry point address
    uint32_t e_phoff; //Program Header table file offset
    uint32_t e_shoff; //section header table file offset
    uint32_t e_flags; //processor-specific flags
    uint16_t e_ehsize; //ELF header size in bytes (should be 52)
    uint16_t e_phentsize; //size of one program table entry
    uint16_t e_phnum; //number of program header entries
    uint16_t e_shentsize; //sizeof one section header table entry
    uint16_t e_shnum; //number of section header entries
    uint16_t e_shstrndx; //section header string table index
} Elf32_Ehdr;
// ELF Program Header Structure
typedef struct {
    uint32_t p_type;   // Type of segment
    uint32_t p_offset; // Offset in file
    uint32_t p_vaddr;  // Virtual address in memory
    uint32_t p_paddr;  // Physical address (ignored on most systems)
    uint32_t p_filesz; // Size of segment in file
    uint32_t p_memsz;  // Size of segment in memory
    uint32_t p_flags;  // Segment flags (R, W, X)
    uint32_t p_align;  // Alignment of segment in memory
} Elf32_Phdr;
// ELF Section Header Structure
typedef struct {
    uint32_t sh_name;       // Name of the section (index into string table)
    uint32_t sh_type;       // Type of the section
    uint32_t sh_flags;      // Section attributes
    uint32_t sh_addr;       // Address in memory
    uint32_t sh_offset;     // Offset in the file
    uint32_t sh_size;       // Size of the section
    uint32_t sh_link;       // Link to another section (usually for symbols)
    uint32_t sh_info;       // Extra information (depends on section type)
    uint32_t sh_addralign;  // Section alignment
    uint32_t sh_entsize;    // Size of each entry (for table sections)
} Elf32_Shdr;
Elf32_Shdr *shdr;
Elf32_Ehdr elfhdr;
Elf32_Phdr *phdr;


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
/* I need to setup all my object file to have 64 byte header
 * I need 32 bytes for each program header
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
    i_type = 0x13,
    i2_typ = 0x03
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
    slt = 0x2,//User determines if the values are signed or not
    sltu = 0x3,
    addi = 0x0,
    xori = 0x4,
    ori = 0x6,
    andi = 0x7,
    slli = 0x1,
    sri = 0x5, //needs funct7
    slti = 0x2,
    sltiu = 0x3,
    lb = 0x0,
    lh = 0x1,
    lw = 0x2,
    lbu = 0x4,
    lhu = 0x5
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
        read_objfile(fopen(argv[j],"rb"));
        
        // if (!read_image(argv[j])) {
        //     printf("Failed to load Image!\n");
        // }
    }
    signal(SIGINT, handle_interrupt);   
    enum { PC_START = 0xc000000};
    reg[pc] = elfhdr.e_entry;
    printf("Entry: 0x%x\n",elfhdr.e_entry);
    int running = 1;
    int count = 0;
    while (running && count <= 8) {
        uint32_t instruction = mem_read(reg[pc]);
        uint8_t opcode = (instruction & 0x7F);
        uint8_t compressed = (instruction & 0x3);
        printf("Instruction: 0x%x\n",instruction);
        // printf("Opcode: 0x%x\n",opcode);
        // printf("count: %d\n",count);
        count++;
        if (compressed != 0x3) {
            printf("compressed!\n");
            reg[pc] += 2;
            switch (compressed) {
                ;;
            }
            continue;
        }
        reg[pc] += 4;
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
                        printf("after lw: %d\n",reg[rd]);
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

void read_objfile(FILE *file) {
    /* In C structs are just heterogenous data structures, its just bytes but the data types do not need to be the same.
     * I complete forgot about that.
     * */
    size_t bytes = fread(&elfhdr,sizeof(elfhdr),1,file);
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
}
