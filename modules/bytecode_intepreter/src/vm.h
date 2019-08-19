#pragma once

//  __attribute__((packed))

typedef enum
{
  //OTHER SHIT
  VM_HALT,
  VM_NOP,
  VM_LOADI,
  VM_LOADF,
  VM_MOV,
  VM_CMP,

  //NAH
  VM_SHR,
  VM_SHL,

  //MATH
  VM_XOR,
  VM_ADD,
  VM_SUB,
  VM_MUL,
  VM_DIV,
  VM_INC,
  VM_DEC,
  VM_AND,
  VM_OR,

  VM_CALL,
  VM_RET,
  VM_JUMP,
  VM_LOOP,
  VM_JZ,
  VM_JNZ,
} vm_opcodes_t;

//  __attribute__((packed))

enum
{
  VM_R0,
  VM_R1,
  VM_R2,
  VM_R3,
  VM_R4,
  VM_R5,
  VM_R6,
  VM_R7,
  VM_R8,
  VM_R9,
  VM_R10,
  VM_R11,
  VM_R12,
  VM_R13,
  VM_R14,
  VM_R15,
  VM_R16,
  VM_REG_COUNT
};

typedef union
{
  char signed_char[4];
  unsigned char unsigned_char;
  short signed_short[2];
  unsigned short unsigned_short[2];
  int sign_numb;
  unsigned int uint_numb;
  float float32;
} vm_register_t;

typedef union
{
  char signed_char[4];
  unsigned char unsigned_char;
  short signed_short[2];
  unsigned short unsigned_short[2];
  int signed_interger;
  unsigned int unsigned_interger;
  float float_32bit;
} vm_register_data_t;

typedef struct
{
  vm_opcodes_t opcode;
  vm_register_data_t registers[VM_REG_COUNT];
} vm_chunk_t;

typedef struct
{
  int halt;
  int zflag;
  unsigned int ip;
  unsigned int ip_old;
  unsigned int instruction_count;
  vm_chunk_t *code;
  vm_register_data_t registers[VM_REG_COUNT];
} vm_struct_t;

vm_chunk_t *vm_parse( vm_struct_t *vm, int argc, char **argv );
vm_struct_t *vm_init();
void vm_clean(vm_struct_t *ptr);
void vm_execute( vm_struct_t *vm );

