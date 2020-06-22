#include <cJSON.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>
#include <time.h>

#include "cmds.h"
#include "events.h"

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
  { "ранд", "рандомное число", cmd_rand },
  { "когда", "узнать дату события", cmd_rand_date },
  { "кто", "выбирает случайного человека из беседы", cmd_who },
  { "инфа", "узнать вероятность чего-либо", cmd_info },
  { "оцени", "оценивает что-либо", cmd_rate },
  { "доки", "ищет документы в вк", cmd_rand_docs },
  { "курс", "курс валют", cmd_valute_course },
  { "флип", "подбросить монетку", cmd_flip },
  { "погода", "показывает погоду сейчас", cmd_weather },
  { "crc32", "подсчитывает crc32 хеш строки или файла", cmd_crc32 },
  { "котик", "рандомный котик", cmd_cat },
  { "Начать", NULL, cmd_start },
#ifdef DEBUG
  { "debug", NULL, cmd_debug },
#endif
  { NULL, NULL, NULL }
};

void Module_Init_Cmds()
{
    for(int i = 0; commands[i].string != NULL; i++)
    REGISTER_COMMAND(&module_info, commands[i].string, commands[i].description, commands[i].function);
}

void Module_Init_Events()
{
    REGISTER_EVENT( "chat_invite_user", chat_invite_user_handler );
    REGISTER_EVENT( "chat_invite_user_by_link", chat_invite_user_handler );
    REGISTER_EVENT( "chat_kick_user", chat_kick_user );
    REGISTER_EVENT_HOOK( "message_new", message_new_hook );
}

module_info_t Module_Init(int apiver, engine_api_t *apifuncs)
{
    memcpy(&engine_api, apifuncs, sizeof(engine_api));

    Module_Init_Cmds();
    Module_Init_Events();
    return module_info;
}
