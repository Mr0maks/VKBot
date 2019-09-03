#include <stdio.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>

engine_api_t engine_api;

void cmd_hello_world(vkapi_handle *obj, vkapi_message_object *message, int argc, char **argv, const char *args )
{
  (*engine_api.vkapi_send_message)(obj, message->peer_id, "Hello from module world!", NULL, 0);
}

typedef union
{
  int i;
  float f;
} floatint_t;

float Q_rsqrt( float number )
{
  floatint_t t;
  float x2, y;
  const float threehalfs = 1.5F;

  x2 = number * 0.5F;
  t.f  = number;
  t.i  = 0x5f3759df - ( t.i >> 1 );               // what the fuck?
  y  = t.f;
  y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration
  //	y  = y * ( threehalfs - ( x2 * y * y ) );   // 2nd iteration, this can be removed

  return y;
}


#include <math.h>

void cmd_sqrt(vkapi_handle *obj, vkapi_message_object *message, int argc, char **argv, const char *args)
{

  if(argc < 1)
    {
      (*engine_api.vkapi_send_message)(obj, message->peer_id, "Использование: sqrt <Число>", NULL, 0);
      return;
    }

  float in = strtof(argv[1], NULL);

  if(in == 0.0f)
    {
      (*engine_api.vkapi_send_message)(obj, message->peer_id, "Ошибка: число равно 0", NULL, 0);
      return;
    }

  double result = (double)sqrtf(in);

  char buff[1024] = { 0 };

  snprintf(buff, 1024, "Корень: %f\n", result);

  (*engine_api.vkapi_send_message)(obj, message->peer_id, buff, NULL, 0);
}

module_info_t module_info =
{
    "hello world",
    "0",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  (*engine_api.register_command)(&module_info, "helloworld", "hello world модуль для демонстрации", cmd_hello_world);
  (*engine_api.register_command)(&module_info, "sqrt", "Находит квадратный корень", cmd_sqrt);
}
