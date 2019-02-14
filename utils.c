#include "utils.h"
#include <stdio.h>
#include <stdlib.h>

//sign extends a bitfield with given size
/* You may find implementing this function helpful */
int bitSigner( unsigned int field, unsigned int size){
    /* YOUR CODE HERE */
    sWord bitfield = field;
    bitfield = ((field << (32-size) ) >> (32-size));
    unsigned int sign = bitfield >> (size - 1);
    sWord extension = 1;
    if (sign == 1) {
      int i = 0;
      while( i < 32 - size){
        extension = extension << 1;
        extension += 1;
        i++;
      }
      extension = extension << size;
      return extension + bitfield;
    }
    return bitfield;
}

/* Remember that the offsets should return the offset in BYTES */

int get_branch_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    Half imm_4_1 = instruction.sbtype.imm5;
    Half imm_11 = instruction.sbtype.imm5;
    Half imm10_5 = instruction.sbtype.imm7;
    Half imm_12 = instruction.sbtype.imm7;
    imm_4_1 = ((imm_4_1 >> 1) << 12) >> 11;
    imm_11 = (imm_11 << 15) >> 4;
    imm10_5 = (imm10_5 << 10) >> 5;
    imm_12 = ((imm_12 >> 6) << 15) >> 3;
    return imm_4_1 + imm10_5 + imm_11 + imm_12;
}

int get_jump_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    Word imm = instruction.ujtype.imm;
    Word imm_10_1 = ((imm << 13) >> 22) << 1;
    Word imm_11 = ((imm << 23) >> 31) << 11;
    Word imm_19_12 = ((imm << 24) >> 24) << 12;
    Word imm_20 = ((imm << 12) >> 31) << 20;
    return imm_10_1 + imm_11 + imm_19_12 + imm_20;
}

int get_store_offset(Instruction instruction) {
    /* YOUR CODE HERE */
    Half imm5 = instruction.stype.imm5;
    Half imm7 = instruction.stype.imm7;
    Half imm_4_0 =  (imm5 << 11) >> 11;
    Half imm_11_5 = (imm7 << 9) >> 4;
    return imm_4_0 + imm_11_5;
}

void handle_invalid_instruction(Instruction instruction) {
    printf("Invalid Instruction: 0x%08x\n", instruction.bits);
}

void handle_invalid_read(Address address) {
    printf("Bad Read. Address: 0x%08x\n", address);
    exit(-1);
}

void handle_invalid_write(Address address) {
    printf("Bad Write. Address: 0x%08x\n", address);
    exit(-1);
}
