#include <enginecallbacks.h>
#include "va_utils.h"
#include "db_core.h"

void cmd_reload(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if(message->private_message)
        return;

    db_chat_warnings_chat_init(message->peer_id);
}

void cmd_profile(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    int priv = USERS_GET_PRIVILAGE(message->from_id);
    string_t s = STRING_INIT();
    STRING_FORMAT(s, "Ваш уровень привелегий в чате - %s\nПредупреждений в чате - %i\n", USERS_GET_PRIVILAGE_NAME(priv), db_chat_warnings_get(message->peer_id, message->from_id));
    VKAPI_SEND_MESSAGE( message->peer_id, s->ptr, NULL, 0);
    STRING_DESTROY(s);
    return;
}

void cmd_set_greeting(vkapi_message_object *message, int argc, char **argv, const char *args)
{

    if(message->private_message)
    {
        VKAPI_SEND_MESSAGE( message->peer_id, "В личных сообщениях команда не работает!\n", NULL, 0);
        return;
    }

    if(db_chat_greetings_push(message->peer_id, args))
    {
        VKAPI_SEND_MESSAGE( message->peer_id, "Приветствие установлено!\n", NULL, 0);
        return;
    }

    VKAPI_SEND_MESSAGE( message->peer_id, "Приветствие не установлено!\n", NULL, 0);
}

void cmd_set_privilage(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    int priv_level = USERS_GET_PRIVILAGE(message->from_id);

    if(priv_level != 2)
    {
        VKAPI_SEND_MESSAGE( message->peer_id, "Недостаточно прав для этой команды!", NULL, 0 );
        return;
    }

    if(!argc || argc > 2)
    {
        usage:
        VKAPI_SEND_MESSAGE( message->peer_id, "Использование: <id> <привилегия>", NULL, 0 );
        return;
    }

    if(!atoi(argv[1]) || !atoi(argv[2]))
        goto usage;

    USERS_SET_PRIVILAGE(atoi(argv[1]), atoi(argv[2]));

    VKAPI_SEND_MESSAGE(message->peer_id, va("Привилегия %s для %i установленна", USERS_GET_PRIVILAGE_NAME(priv_level), atoi(argv[2])), NULL, 0);
}
