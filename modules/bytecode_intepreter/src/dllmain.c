#include <stdio.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>

#include "vm.h"

engine_api_t engine_api;

void cmd_bytecode_compile(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{

}

void cmd_bytecode(vkapi_handle *object, vkapi_message_object *message, int argc, char **argv, const char *args)
{
  vm_struct_t *vm = vm_init();

  vm_chunk_t *ptr = vm_parse(vm, argc, argv);

  if(!ptr)
    {
    vm_clean(vm);
    return;
    }

  vm_execute(vm);

  (*engine_api.vkapi_send_message)(object, message->peer_id, "Если вы это видите то скорее всего байткод выполнился\n");

  vm_clean(vm);

  free(ptr);
}

void Module_Init(engine_api_t *api, int ver)
{
  memcpy(&engine_api, api, sizeof(engine_api));

  (*engine_api.register_command)("bytecodecompile", "bytecode compiler for bytecode", cmd_bytecode_compile );
  (*engine_api.register_command)("bytecode", "bytecode intepreter", cmd_bytecode);

}
