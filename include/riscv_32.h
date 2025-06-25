#ifndef RISCV_32_H
#define RISCV_32_H

#include <stdint.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

#define MEMORY_MAX ((uint64_t)1 << 30)
#define ELF_HEADER 52
#define PROGRAM_HEADER 32
#define PT_LOAD 1
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

//function prototypes
void handle_interrupt(int signal);
void checkRegister(int reg);
uint32_t bit_extend(uint32_t val, int bitcnt);
int read_objfile(FILE *file);
uint32_t mem_read(uint32_t address);


//Instruction types
enum {
    ci_css_cr = 0x2,
    cl_ciw = 0x0,
    ci_cj_cb_cs = 0x1,
    r_type = 0x33,
    i_type = 0x13,
    i2_typ = 0x03,
    s_type = 0x23,
    u_type = 0x37,
    u2_typ = 0x17,
    i3_typ = 0x73,
    b_type = 0x63,
    j_type = 0x6f,
    jalr = 0x67
};
//Instructions
enum 
{
    arith = 0x0,
    xor = 0x4,
    or = 0x6,
    and = 0x7,
    sll = 0x1,
    sr = 0x5,
    slt = 0x2,
    sltu = 0x3,
    addi = 0x0,
    xori = 0x4,
    ori = 0x6,
    andi = 0x7,
    slli = 0x1,
    sri = 0x5, 
    slti = 0x2,
    sltiu = 0x3,
    lb = 0x0,
    lh = 0x1,
    lw = 0x2,
    lbu = 0x4,
    lhu = 0x5,
    add = 0x00,
    sub = 0x20,
    srl = 0x00,
    sra = 0x20,
    srli = 0x00,
    srai = 0x20,
    sb = 0x0,
    sh = 0x1,
    sw = 0x2,
    clwsp = 0x2,
    cswsp = 0x6,
    clw = 0x2,
    csw = 0x6,
    cj = 0x5,
    cjal = 0x1,
    cjr = 0x8, 
    cjalr = 0x9,
    cbeqz = 0x6,
    cbnez = 0x7,
    cli = 0x2,
    clui = 0x3,
    caddi = 0x0,
    caddi16sp = 0x3,
    caddi4spn = 0x0,
    cmv = 0x8,
    cadd = 0x9,
    cnop = 0x0,
    cebreak = 0x9,
    cslli = 0x0,
    cb_cs = 0x4,
    csrai = 0x0,
    csrli = 0x1,
    candi = 0x2,
    cand = 0x8f,
    cor =  0x8e,
    cxor = 0x8d,
    csub = 0x8c,
    beq = 0x0,
    bne = 0x1,
    blt = 0x4,
    bge = 0x5,
    bltu = 0x6,
    bgeu = 0x7
};


// Registers
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

#endif