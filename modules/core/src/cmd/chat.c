#include <enginecallbacks.h>
#include "va_utils.h"
#include "db_core.h"
#include "core_vkapi.h"

int vkapi_get_id_from(char *str);

void cmd_reload(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    if(message->private_message)
        return;

    if(USERS_GET_PRIVILAGE(message->from_id) != 3)
        return;

    db_chat_warnings_chat_init(message->peer_id);
}

void cmd_profile(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    string_t s = STRING_INIT();
    STRING_FORMAT(s, "Ваш уровень привелегий в чате - %s\nПредупреждений в чате - %i\n", USERS_GET_PRIVILAGE_NAME(db_chat_get_privilage(message->peer_id, message->from_id)), db_chat_warnings_get(message->peer_id, message->from_id));
    VKAPI_SEND_MESSAGE( message->peer_id, s->ptr, NULL, 0);
    STRING_DESTROY(s);
    return;
}

void cmd_warn(vkapi_message_object *message, int argc, char **argv, const char *args )
{
    if(!argv[1])
    {
        VKAPI_SEND_MESSAGE(message->peer_id, "Не указан пользователь", NULL, 0);
        return;
    } else if(argv[1])
    {
        if(!atoi(argv[1]) || !vkapi_get_id_from(argv[1]))
            VKAPI_SEND_MESSAGE(message->peer_id, "Не указан пользователь", NULL, 0);
        return;
    }

    int member_id = atoi(argv[1]);

    if(!member_id)
        member_id = vkapi_get_id_from(argv[1]);

    db_chat_warnings_inc(message->peer_id, member_id);
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
    int priv_level = db_chat_get_privilage(message->peer_id, message->from_id);

    if(vkapi_is_chat_admin(message->peer_id, message->from_id))
        priv_level = 2;

    if(priv_level < 2)
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

    if(!atoi(argv[1]))
        goto usage;

    db_chat_set_privilage(message->peer_id, atoi(argv[1]), atoi(argv[2]));

    VKAPI_SEND_MESSAGE(message->peer_id, va("Привилегия %s для %i установленна", USERS_GET_PRIVILAGE_NAME(atoi(argv[2])), atoi(argv[1])), NULL, 0);
}
