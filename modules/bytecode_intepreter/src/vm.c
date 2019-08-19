#include <stdlib.h>
#include <enginecallbacks.h>
#include <module_api.h>
#include "vm.h"
#include <string.h>
#include <stdio.h>

vm_chunk_t *vm_parse(vm_struct_t *vm, int argc, char **argv )
{
  if((argc - 1) % 4)
    {
      return NULL;
    }

  size_t need_alloc = (argc - 1) / 4;

  printf("argc: %i, need alloc %lu\n", argc, need_alloc);

  vm_chunk_t *ptr = calloc(need_alloc, sizeof(vm_chunk_t));

  if(!ptr)
  return NULL;

  unsigned int chunk = 0;

  for (int i = 1; i < argc ; i++ )
  {
    sscanf(argv[i], "%i", &ptr[chunk].opcode);
    i++;
    sscanf(argv[i], "%x", &ptr[chunk].registers[VM_R0].unsigned_interger);
    i++;
    sscanf(argv[i], "%x", &ptr[chunk].registers[VM_R1].unsigned_interger);
    i++;
    sscanf(argv[i], "%x", &ptr[chunk].registers[VM_R2].unsigned_interger);

    printf("vm: %i %X %X %X\n", ptr[chunk].opcode, ptr[chunk].registers[VM_R0].unsigned_interger, ptr[chunk].registers[VM_R1].unsigned_interger, ptr[chunk].registers[VM_R2].unsigned_interger);

    chunk++;
  }

  vm->code = ptr;
  vm->instruction_count = chunk;
  return ptr;
}

void vm_exec_instruction(vm_struct_t *vm, vm_chunk_t *inst)
{
  switch (inst->opcode)
    {
      case VM_HALT:
	{
	  printf("halt\n");
	  vm->halt = 1;
	  break;
	}

      case VM_NOP:
	{
	  printf("nop\n");
	  break;
	}

      case VM_LOADI:
	{
	  int reg = inst->registers[1].signed_interger;
	  printf("loadi %d, r%d\n", inst->registers[0].signed_interger, reg);
	  vm->registers[reg].signed_interger = inst->registers[0].signed_interger;
	  break;
	}
      case VM_LOADF:
	{
	  int reg = inst->registers[1].signed_interger;
	  printf("loadf %f, r%d\n", inst->registers[0].float_32bit, reg);
	  vm->registers[reg].float_32bit = inst->registers[0].float_32bit;
	  break;
	}

      case VM_MOV:
	{
	  printf("mov r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger );
	  memcpy(&vm->registers[inst->registers[1].signed_interger], &vm->registers[inst->registers[0].signed_interger], sizeof(vm_register_data_t));
	  break;
	}

      case VM_CMP:
	{
	  int result = 0;
	  printf("cmp r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger );

	  if(vm->registers[inst->registers[0].signed_interger].unsigned_interger == vm->registers[inst->registers[1].signed_interger].unsigned_interger)
	    {
	      result = 1;
	    }

	  vm->zflag = result;
	  break;
	}

      case VM_SHR:
	{
	  printf("shr r%d, %d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger);
	  vm->registers[inst->registers[0].signed_interger].unsigned_interger = (vm->registers[inst->registers[0].signed_interger].unsigned_interger >> inst->registers[1].signed_interger);
	  break;
	}

      case VM_SHL:
	{
	  printf("shl r%d, %d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger);
	  vm->registers[inst->registers[0].signed_interger].unsigned_interger = vm->registers[inst->registers[0].signed_interger].unsigned_interger << inst->registers[1].signed_interger;
	  break;
	}

      case VM_XOR:
	{
	  int reg = inst->registers[2].signed_interger;
	  printf("xor r%d, r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger, inst->registers[2].signed_interger);
	  vm->registers[reg].unsigned_interger = vm->registers[inst->registers[0].signed_interger].unsigned_interger ^ vm->registers[inst->registers[1].signed_interger].unsigned_interger;
	  break;
	}

      case VM_ADD:
	{
	  int reg = inst->registers[2].signed_interger;
	  printf("add r%d, r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger, inst->registers[2].signed_interger);
	  vm->registers[reg].signed_interger = vm->registers[inst->registers[0].signed_interger].signed_interger + vm->registers[inst->registers[1].signed_interger].signed_interger;
	  break;
	}

      case VM_SUB:
	{
	  int reg = inst->registers[2].signed_interger;
	  printf("sub r%d, r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger, inst->registers[2].signed_interger);
	  vm->registers[reg].signed_interger = vm->registers[inst->registers[0].signed_interger].signed_interger - vm->registers[inst->registers[1].signed_interger].signed_interger;
	  break;
	}

      case VM_MUL:
	{
	  int reg = inst->registers[2].signed_interger;
	  printf("mul r%d, r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger, inst->registers[2].signed_interger);
	  vm->registers[reg].signed_interger = vm->registers[inst->registers[0].signed_interger].signed_interger * vm->registers[inst->registers[1].signed_interger].signed_interger;
	  break;
	}

      case VM_INC:
	{
	  printf("inc r%d\n", inst->registers[0].signed_interger);
	  vm->registers[inst->registers[0].signed_interger].signed_interger++;
	  break;
	}
      case VM_DEC:
	{
	  printf("dec r%d\n", inst->registers[0].signed_interger );
	  vm->registers[inst->registers[0].signed_interger].signed_interger--;
	  break;
	}

      case VM_AND:
	{
	  printf("and r%d, r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger, inst->registers[2].signed_interger );
	  vm->registers[inst->registers[2].signed_interger].signed_interger = vm->registers[inst->registers[0].signed_interger].signed_interger & vm->registers[inst->registers[1].signed_interger].signed_interger;
	  break;
	}

      case VM_OR:
	{
	  int reg = inst->registers[2].signed_interger;
	  printf("or r%d, r%d, r%d\n", inst->registers[0].signed_interger, inst->registers[1].signed_interger, inst->registers[2].signed_interger);
	  vm->registers[reg].signed_interger = vm->registers[inst->registers[0].signed_interger].signed_interger | vm->registers[inst->registers[1].signed_interger].signed_interger;
	  break;
	}

      case VM_CALL:
	{
	  if(inst->registers[0].unsigned_interger > vm->instruction_count)
	    {
	      vm->halt = 1;
	      break;
	    }

	  printf("call %u\n", inst->registers[0].unsigned_interger);
	  vm->ip_old = vm->ip;
	  vm->ip = inst->registers[0].unsigned_interger;
	  break;
	}
      case VM_RET:
	{
	  printf("ret\n");
	  vm->ip = vm->ip_old;
	  break;
	}
      case VM_JUMP:
	{
	  if(inst->registers[0].unsigned_interger > vm->instruction_count)
	    {
	      vm->halt = 1;
	      break;
	    }

	  printf("jump %ui\n", inst->registers[0].unsigned_interger);
	  vm->ip = inst->registers[0].unsigned_interger;
	  break;
	}
      case VM_LOOP:
	{
	  printf("loop %i (r16 is %i)\n", inst->registers[0].unsigned_interger, vm->registers[VM_R16].unsigned_interger );

	  if(inst->registers[0].unsigned_interger > vm->instruction_count)
	    {
	      vm->halt = 1;
	      break;
	    }

	  if(vm->registers[VM_R16].unsigned_interger > 0)
	    {
	      vm->registers[VM_R16].unsigned_interger--;
	      vm->ip = inst->registers[0].unsigned_interger;
	    }

	  break;
	}
      case VM_JZ:
	{
	  if(vm->zflag == 0)
	    break;

	  if(inst->registers[0].unsigned_interger > vm->instruction_count)
	    {
	      vm->halt = 1;
	      break;
	    }

	  printf("jz %ui\n", inst->registers[0].unsigned_interger);
	  vm->ip = inst->registers[0].unsigned_interger;
	  break;
	}
      case VM_JNZ:
	{
	  if(vm->zflag == 1)
	    break;

	  if(inst->registers[0].unsigned_interger > vm->instruction_count)
	    {
	      vm->halt = 1;
	      break;
	    }

	  printf("jnz %ui\n", inst->registers[0].unsigned_interger);
	  vm->ip = inst->registers[0].unsigned_interger;
	  break;
	}

      default:
	{
	  printf("invalid opcode: 0x%X", inst->opcode);
	  vm->halt = 1;
	  break;
	}

    }
}

vm_struct_t *vm_init()
{
  vm_struct_t *ptr = calloc(1, sizeof(vm_struct_t));
  return ptr;
}

void vm_clean(vm_struct_t *ptr)
{
  free(ptr);
}

void vm_execute( vm_struct_t *vm )
{
  while (vm->ip != vm->instruction_count && vm->halt == 0)
    {
      vm_chunk_t *ptr = &vm->code[vm->ip];
      vm_exec_instruction(vm, ptr);
      vm->ip++;
    }

  if(vm->halt == 1)
    printf("vm: halt call/instruction exeption\n");

  printf("REG DUMP:\n");

  for (int i = 0; i < VM_REG_COUNT ; i++) {
      printf("r%i = 0x%X\n", i, vm->registers[i].unsigned_interger );
    }
}
