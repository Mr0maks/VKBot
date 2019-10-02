#include <stdio.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>

#include "cmds.h"

engine_api_t engine_api;

module_info_t module_info =
{
    "core",
    "0.1",
    __DATE__,
    "https://github.com/Mr0maks",
    "Mr0maks",
    ENGINE_API_VERSION
};

const cmds_t commands[] = {
  { "ping", "команда для проверки бота на отзывчевость", cmd_ping },
  { "b64e", "кодирует строку в формате base64", cmd_base64_encode },
  { "b64d", "декодирует строку в формате base64", cmd_base64_decode },
  { "ранд", "рандомное число", cmd_rand },
  { "когда", "узнать дату события", cmd_rand_date },
//  { "кто", "выберает рандомного человека из беседы (нужны права администратора)", cmd_who },
  { "инфа", "узнать вероятность чего-либо", cmd_info },
  { "оцени", "оценивает что-либо", cmd_rate },
//  { "выбери", "выбирает что то одно", cmd_choose },
  { "доки", "ищет документы в вк", cmd_rand_docs },
  { "курс", "курс валют", cmd_valute_curse },
  { "флип", "подбросить монетку", cmd_flip },
  { "погода", "показывает погоду сейчас", cmd_weather },
  { "crc32", "подсчитывает crc32 хеш строки или файла", cmd_crc32 },
  { "хлмемы", "годный плейлист ютуба с мемами хл", cmd_hlmemes },
  { "котик", "рандомный котик", cmd_cat },
  { "профиль", "показывает привелегию и всо", cmd_get_privilage },
#ifdef DEBUG
  { "debug", "бот собран с отладочными функциями", cmd_debug },
#endif
  { NULL, NULL, NULL }
};

void Module_Init_Cmds()
{
    for(int i = 0; commands[i].string != NULL; i++)
    REGISTER_COMMAND(&module_info, commands[i].string, commands[i].description, commands[i].function);
}

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
  memcpy(&engine_api, apifuncs, sizeof(engine_api));
  *info = &module_info;

  Module_Init_Cmds();
}
