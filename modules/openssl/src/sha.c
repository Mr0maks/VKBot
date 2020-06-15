#include <enginecallbacks.h>
#include "va_utils.h"
#include <strings.h>
#include <string.h>

#include <cJSON.h>
#include <openssl/sha.h>

void cmd_sha1(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    SHA_CTX ctx;
    unsigned char sha1_buff[SHA_DIGEST_LENGTH];
    if(!argc && !message->attachmens)
    {
    usage:
        VKAPI_SEND_MESSAGE( message->peer_id, "Использование: sha1 <строка> или прикреплённые документы", NULL, 0);
        return;
    }

    if(argc)
    {
        SHA1_Init(&ctx);
        SHA1_Update(&ctx, args, strlen(args));
        SHA1_Final(sha1_buff, &ctx);

        const char *digest = NULL;
        for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
        {
            if(digest == NULL) digest = va("%02x", sha1_buff[i]);
            else digest = va("%s%02x", digest, sha1_buff[i]);
        }

        VKAPI_SEND_MESSAGE( message->peer_id, va("SHA1 хеш строки: %s", digest), NULL, 0);
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

            SHA1_Init(&ctx);
            SHA1_Update(&ctx, file->ptr, file->len);
            SHA1_Final(sha1_buff, &ctx);

            const char *digest = NULL;
            for (int i = 0; i < SHA_DIGEST_LENGTH; i++)
            {
                if(digest == NULL) digest = va("%02x", sha1_buff[i]);
                else digest = va("%s%02x", digest, sha1_buff[i]);
            }

            char *ptr = va("SHA1 хеш файла %s: %s\n", cJSON_GetStringValue(cJSON_GetObjectItem(doc, "title")), digest );

            STRING_STRNCAT(msg, ptr, strlen(ptr));

            STRING_DESTROY(file);
        }

        VKAPI_SEND_MESSAGE( message->peer_id, msg->ptr, NULL, 0);
        STRING_DESTROY(msg);
    }
}


void cmd_sha256(vkapi_message_object *message, int argc, char **argv, const char *args)
{
    SHA256_CTX ctx;
    unsigned char sha1_buff[SHA256_DIGEST_LENGTH];
    if(!argc && !message->attachmens)
    {
    usage:
        VKAPI_SEND_MESSAGE( message->peer_id, "Использование: sha256 <строка> или прикреплённые документы", NULL, 0);
        return;
    }

    if(argc)
    {
        SHA256_Init(&ctx);
        SHA256_Update(&ctx, args, strlen(args));
        SHA256_Final(sha1_buff, &ctx);

        const char *digest = NULL;
        for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
        {
            if(digest == NULL) digest = va("%02x", sha1_buff[i]);
            else digest = va("%s%02x", digest, sha1_buff[i]);
        }

        VKAPI_SEND_MESSAGE( message->peer_id, va("SHA256 хеш строки: %s", digest), NULL, 0);
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

            SHA256_Init(&ctx);
            SHA256_Update(&ctx, file->ptr, file->len);
            SHA256_Final(sha1_buff, &ctx);

            const char *digest = NULL;
            for (int i = 0; i < SHA256_DIGEST_LENGTH; i++)
            {
                if(digest == NULL) digest = va("%02x", sha1_buff[i]);
                else digest = va("%s%02x", digest, sha1_buff[i]);
            }

            char *ptr = va("SHA256 хеш файла %s: %s\n", cJSON_GetStringValue(cJSON_GetObjectItem(doc, "title")), digest );

            STRING_STRNCAT(msg, ptr, strlen(ptr));

            STRING_DESTROY(file);
        }

        VKAPI_SEND_MESSAGE( message->peer_id, msg->ptr, NULL, 0);
        STRING_DESTROY(msg);
    }
}
