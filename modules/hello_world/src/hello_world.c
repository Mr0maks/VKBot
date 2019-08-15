#include <stdio.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>

engine_api_t engine_api;

void cmd_hello_world(vkapi_handle *obj, vkapi_message_object *message, int argc, char **argv, const char *args )
{
  (*engine_api.vkapi_send_message)(obj, message->peer_id, "Hello from module world!");
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
      (*engine_api.vkapi_send_message)(obj, message->peer_id, "Использование: sqrt <Число>");
      return;
    }

  float in = strtof(argv[1], NULL);

  if(in == 0.0f)
    {
      (*engine_api.vkapi_send_message)(obj, message->peer_id, "Ошибка: число равно 0");
      return;
    }

  double result = (double)sqrtf(in);

  char buff[1024] = { 0 };

  snprintf(buff, 1024, "Корень: %f\n", result);

  (*engine_api.vkapi_send_message)(obj, message->peer_id, buff);

}

void Module_Init(engine_api_t *api, int ver)
{
  memcpy(&engine_api, api, sizeof(engine_api));

  (*engine_api.register_command)("helloworld", "hello world модуль для демонстрации", cmd_hello_world);
  (*engine_api.register_command)("sqrt", "Находит квадратный корень", cmd_sqrt);
}
