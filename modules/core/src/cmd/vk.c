#include <enginecallbacks.h>

void cmd_start(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    VKAPI_SEND_MESSAGE(message->peer_id, "Привет! Я Максбот - минимальный чат менеджер написанный на C. Для полной справки напишите максбот помощь", NULL, 0);
}
