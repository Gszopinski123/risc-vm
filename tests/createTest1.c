#include <stdio.h>
#include <stdint.h>

int main(void) {
    //lets see
    printf("Starting Test!\n");
    uint32_t instr[32];
    //max size of memory 4294967296
    // starting point should be about 18.25% in to memory
    instr[0]  = 0x30000000 / 4;//starting pc address
    instr[1]  = 0x3e800093;//addi
    instr[2]  = 0x7580c113;//xori
    instr[3]  = 0x67316193;//ori
    instr[4]  = 0x5121f213;//andi
    instr[5]  = 0x00421293;//slli
    instr[6]  = 0x0062d313;//srli
    instr[7]  = 0x40235393;//srai
    instr[8]  = 0x6743a413;//slti
    instr[9]  = 0x17243493;//sltiu
    instr[10] = 0x008000b3;//add
    instr[11] = 0x402000b3;//sub
    instr[12] = 0x008040b3;//xor
    instr[13] = 0x008060b3;//or
    instr[14] = 0x008070b3;//and
    instr[15] = 0x008010b3;//sll
    instr[16] = 0x008050b3;//srl
    instr[17] = 0x408050b3;//sra
    instr[18] = 0x008020b3;//slt
    instr[19] = 0x008030b3;//sltu
    FILE* file = fopen("testfiles/test1.obj","w");
    if (!file) {return 1;}
    fwrite(instr,sizeof(uint32_t),20,file);
    fclose(file);
    return 0;
}