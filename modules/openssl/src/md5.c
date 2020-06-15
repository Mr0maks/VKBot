#include <enginecallbacks.h>
#include "va_utils.h"
#include <strings.h>
#include <string.h>

#include <cJSON.h>
#include <openssl/md5.h>

void cmd_md5(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    MD5_CTX ctx;
    unsigned char sha1_buff[MD5_DIGEST_LENGTH];
    if(!argc && !message->attachmens)
    {
    usage:
        VKAPI_SEND_MESSAGE( message->peer_id, "Использование: md5 <строка> или прикреплённые документы", NULL, 0);
        return;
    }

    if(argc)
    {
        MD5_Init(&ctx);
        MD5_Update(&ctx, args, strlen(args));
        MD5_Final(sha1_buff, &ctx);

        const char *digest = NULL;
        for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
        {
            if(digest == NULL) digest = va("%02x", sha1_buff[i]);
            else digest = va("%s%02x", digest, sha1_buff[i]);
        }

        VKAPI_SEND_MESSAGE( message->peer_id, va("MD5 хеш строки: %s", digest), NULL, 0);
    }
    else
    {
        string_t msg = STRING_INIT();

        cJSON *attach = NULL;

        cJSON_ArrayForEach(attach, (cJSON*)message->attachmens)
        {

            cJSON *doc = cJSON_GetObjectItem(attach, "doc");

            if(!doc)
            {
                STRING_DESTROY(msg);
                goto usage;
            }

            string_t file = STRING_INIT();

            CURL_GET(NULL, cJSON_GetStringValue(cJSON_GetObjectItem(doc, "url")), NULL, file);

            MD5_Init(&ctx);
            MD5_Update(&ctx, file->ptr, file->len);
            MD5_Final(sha1_buff, &ctx);

            const char *digest = NULL;
            for (int i = 0; i < MD5_DIGEST_LENGTH; i++)
            {
                if(digest == NULL) digest = va("%02x", sha1_buff[i]);
                else digest = va("%s%02x", digest, sha1_buff[i]);
            }

            char *ptr = va("MD5 хеш файла %s: %s\n", cJSON_GetStringValue(cJSON_GetObjectItem(doc, "title")), digest );

            STRING_STRNCAT(msg, ptr, strlen(ptr));

            STRING_DESTROY(file);
        }

        VKAPI_SEND_MESSAGE( message->peer_id, msg->ptr, NULL, 0);
        STRING_DESTROY(msg);
    }
}
