#include <stdio.h> // for stderr
#include <stdlib.h> // for exit()
#include "types.h"
#include "utils.h"
#include "riscv.h"

void execute_rtype(Instruction, Processor *);
void execute_itype_except_load(Instruction, Processor *);
void execute_branch(Instruction, Processor *);
void execute_jal(Instruction, Processor *);
void execute_load(Instruction, Processor *, Byte *);
void execute_store(Instruction, Processor *, Byte *);
void execute_ecall(Processor *, Byte *);
void execute_lui(Instruction, Processor *);

void execute_instruction(Instruction instruction,Processor *processor,Byte *memory) {
    /* YOUR CODE HERE: COMPLETE THE SWITCH STATEMENTS */
    switch(instruction.opcode) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x03:
          execute_load(instruction, processor, memory);
          processor->PC += 4;
          break;
        case 0x13:
          execute_itype_except_load(instruction, processor);
          processor->PC += 4;
          break;
        case 0x73:
          execute_ecall(processor, memory);
          processor->PC += 4;
          break;
        case 0x33:
          execute_rtype(instruction, processor);
          processor->PC += 4;
          break;
        case 0x23:
          execute_store(instruction, processor, memory);
          processor->PC += 4;
          break;
        case 0x63:
          execute_branch(instruction, processor);
          break;
        case 0x37:
          execute_lui(instruction, processor);
          processor->PC += 4;
          break;
        case 0x6F:
          execute_jal(instruction, processor);
          break;
        default: // undefined opcode
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_rtype(Instruction instruction, Processor *processor) {
  sWord rs1 = processor->R[instruction.rtype.rs1];
  sWord rs2 = processor->R[instruction.rtype.rs2];
  switch(instruction.rtype.funct3) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x0:
          switch(instruction.rtype.funct7){
            case 0x00: /* add */
              processor->R[instruction.rtype.rd] = rs1 + rs2;
              break;
            case 0x01: /* mul */
              {
                sWord result = rs1 * rs2;
                processor->R[instruction.rtype.rd] = result;
                break;
              }
            case 0x20: /* sub */
              processor->R[instruction.rtype.rd] = rs1 - rs2;
              break;
            default:
              handle_invalid_instruction(instruction);
              break;
          }
          break;
        case 0x1:
          switch (instruction.rtype.funct7) {
            case 0x00: /* sll */
              processor->R[instruction.rtype.rd] = rs1 << rs2;
              break;
            case 0x01: /* mulh */
              {
                sDouble product = (rs1 * rs2) >> 32;
                sWord mulh_result = product;
                processor->R[instruction.rtype.rd] = mulh_result;
                break;
              }
            default:
              handle_invalid_instruction(instruction);
              break;
          }
          break;
        case 0x2: /* slt */
          if (instruction.rtype.funct7 == 0x00){
            processor->R[instruction.rtype.rd] = rs1 < rs2;
            break;
          }else{
            handle_invalid_instruction(instruction);
          }
          break;
        case 0x4:
          switch (instruction.rtype.funct7) {
            case 0x00: /* xor */
              processor->R[instruction.rtype.rd] = rs1 ^ rs2;
              break;
            case 0x01: /* div */
              processor->R[instruction.rtype.rd] = rs1 / rs2;
              break;
            default:
              handle_invalid_instruction(instruction);
              break;
            }
            break;
        case 0x5:
          switch (instruction.rtype.funct7) {
            Word rs2_unsigned = rs2;
            case 0x00: /* srl */
              processor->R[instruction.rtype.rd] = rs1 >> rs2_unsigned;
              break;
            case 0x20: /* sra */
              processor->R[instruction.rtype.rd] = rs1 >> rs2_unsigned;
              processor->R[instruction.rtype.rd] = bitSigner(processor->R[instruction.rtype.rd], 32-rs2);
              break;
            default:
              handle_invalid_instruction(instruction);
              break;
            }
            break;
        case 0x6:
          switch (instruction.rtype.funct7) {
            case 0x00: /* or */
              processor->R[instruction.rtype.rd] = rs1 | rs2;
              break;
            case 0x01: /* rem */
              processor->R[instruction.rtype.rd] = rs1 % rs2;
              break;
            default:
              handle_invalid_instruction(instruction);
              break;
            }
            break;
        case 0x7: /* and */
          if (instruction.rtype.funct7 == 0x00){
            processor->R[instruction.rtype.rd] = rs1 & rs2;
          }else{
            handle_invalid_instruction(instruction);
          }
          break;
        default:
            handle_invalid_instruction(instruction);
            exit(-1);
            break;
    }
}

void execute_itype_except_load(Instruction instruction, Processor *processor) {
  int shiftOp;
  shiftOp = 5;
  sWord rs1 = processor->R[instruction.itype.rs1];
  sWord imm = bitSigner(instruction.itype.imm, 12);
  sWord imm_srai = bitSigner(instruction.itype.imm, 6);
  Word imm_srli = bitSigner(instruction.itype.imm,6);
  switch(instruction.itype.funct3) { // What do we switch on?
      /* YOUR CODE HERE */
      case 0x0: /* addi */
        processor->R[instruction.itype.rd] = rs1 + imm;
        break;
      case 0x1: /* slli */
        if (instruction.itype.imm >> shiftOp == 0x00){
          processor->R[instruction.itype.rd] = rs1 << imm;
          break;
        } else {
          handle_invalid_instruction(instruction);
          break;
        }
      case 0x2: /* slti */
        processor->R[instruction.itype.rd] = rs1 < imm;
        break;
      case 0x4: /* xori */
        processor->R[instruction.itype.rd] = rs1 ^ imm;
        break;
      case 0x5:
        switch (instruction.itype.imm >> shiftOp) {
          case 0x00: /* srli */
            processor->R[instruction.itype.rd] = rs1 >> imm_srli;
            break;
          case 0x20: /* srai */
            processor->R[instruction.itype.rd] = bitSigner(rs1 >> imm_srai, 32 - imm_srai);
            break;
          default:
            handle_invalid_instruction(instruction);
            break;
        }
        break;
      case 0x6: /* ori */
        processor->R[instruction.itype.rd] = rs1 | imm;
        break;
      case 0x7: /* andi */
        processor->R[instruction.itype.rd] = rs1 & imm;
        break;
      default:
        handle_invalid_instruction(instruction);
        break;
    }
}

void execute_ecall(Processor *p, Byte *memory) {
    switch(p->R[10]) { // What do we switch on?
        /* YOUR CODE HERE */
        case 0x1:
          printf("%d", p->R[11]);
          break;
        case 0xa:
          printf("exiting the simulator\n");
          exit(0);
          break;
        default: // undefined ecall
            printf("Illegal ecall number %d\n", -1); // What stores the ecall arg?
            exit(-1);
            break;
    }
}

void execute_branch(Instruction instruction, Processor *processor) {
    int branchaddr;
    branchaddr = bitSigner(get_branch_offset(instruction),13);
    /* Remember that the immediate portion of branches
       is counting half-words, so make sure to account for that. */
       switch(instruction.sbtype.funct3) { // What do we switch on?
           /* YOUR CODE HERE */
           case 0x0: /* beq */
             if (processor->R[instruction.sbtype.rs1] == processor->R[instruction.sbtype.rs2]){
               processor->PC += branchaddr;
             } else {
               processor->PC += 4;
             }
             break;
           case 0x1: /* bne */
             if (processor->R[instruction.sbtype.rs1] != processor->R[instruction.sbtype.rs2]){
               processor->PC += branchaddr;
             } else {
               processor->PC += 4;
             }
             break;
           default:
             handle_invalid_instruction(instruction);
             exit(-1);
             break;
    }
}

void execute_load(Instruction instruction, Processor *processor, Byte *memory) {
  switch(instruction.itype.funct3) { // What do we switch on?
      /* YOUR CODE HERE */
      case 0x0: /* lb */
        processor->R[instruction.itype.rd] = bitSigner(load(memory, processor->R[instruction.itype.rs1] + bitSigner(instruction.itype.imm, 12), LENGTH_BYTE, 0), 8);
        break;
      case 0x1: /* lh */
        processor->R[instruction.itype.rd] = bitSigner(load(memory, processor->R[instruction.itype.rs1] + bitSigner(instruction.itype.imm, 12), LENGTH_HALF_WORD, 0), 16);
        break;
      case 0x2: /* lw */
        processor->R[instruction.itype.rd] = load(memory, processor->R[instruction.itype.rs1] + bitSigner(instruction.itype.imm, 12), LENGTH_WORD, 0);
        break;
      default:
        handle_invalid_instruction(instruction);
        break;
    }
}

void execute_store(Instruction instruction, Processor *processor, Byte *memory) {
  switch(instruction.stype.funct3) { // What do we switch on?
      /* YOUR CODE HERE */
      case 0x0: /* sb */
        store(memory,processor->R[instruction.stype.rs1] + get_store_offset(instruction), LENGTH_BYTE, processor->R[instruction.stype.rs2], 0);
        break;
      case 0x1: /* sh */
        store(memory,processor->R[instruction.stype.rs1] + get_store_offset(instruction), LENGTH_HALF_WORD, processor->R[instruction.stype.rs2], 0);
        break;
      case 0x2: /* sw */
        store(memory,processor->R[instruction.stype.rs1] + get_store_offset(instruction), LENGTH_WORD, processor->R[instruction.stype.rs2], 0);
        break;
      default:
        handle_invalid_instruction(instruction);
        exit(-1);
        break;
    }
}

void execute_jal(Instruction instruction, Processor *processor) {
    int nextPC;
    nextPC = bitSigner(get_jump_offset(instruction), 21);
    /* YOUR CODE HERE */
    processor->R[instruction.ujtype.rd] = processor->PC + 4;
    processor->PC += nextPC;
}

void execute_lui(Instruction instruction, Processor *processor) {
    int imm;
    imm = instruction.utype.imm << 12;
    /* YOUR CODE HERE */
    processor->R[instruction.utype.rd] = imm;
}

/* Checks that the address is aligned correctly */
int check(Address address,Alignment alignment) {
    if(address>0 && address < MEMORY_SPACE){
        if(alignment == LENGTH_BYTE){
            return 1;
        }
        else if( alignment == LENGTH_HALF_WORD ){
            return address%2 == 0;
        }
        else if (alignment == LENGTH_WORD){
            return address%4 ==0;
        }
    }

    return 0;

}

void store(Byte *memory,Address address,Alignment alignment,Word value, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
        handle_invalid_write(address);
    }
    /* YOUR CODE HERE */
    Byte data = value;
    memory[address] = data;
    int i = 0;
    for (i = 1; i < alignment; i++){
      data = (value >> (8 * i));
      memory[address + i] = data;
    }
}

Word load(Byte *memory,Address address,Alignment alignment, int check_align) {
    if((check_align && !check(address,alignment)) || (address >= MEMORY_SPACE)) {
    handle_invalid_read(address);
    }

    /* YOUR CODE HERE */
    uint32_t data = 0; // initialize our return value to zero
    uint32_t mem_data = 0;
    data = memory[address];
    int i = 0;
    for (i = 1; i < alignment; i++){
      mem_data = memory[address + i];
      data = data + (mem_data << (i*8));
    }
    return data;
}
