#include "common.h"

static curl_keyvalue_t *curl_keyvalue_new(const char *key, const char *value)
{
    curl_keyvalue_t *ptr = calloc(1, sizeof(curl_keyvalue_t));
    ptr->key = key;
    ptr->value = value;
    ptr->next = NULL;
    return ptr;
}

void curl_postfield_push(curl_postfield_t pool, const char *key, const char *value)
{
    assert(pool);
    assert(key);
    assert(value);

    curl_keyvalue_t *ptr = curl_keyvalue_new(key, value);

    ptr->next = pool->pool;
    pool->pool = ptr;
}

curl_postfield_t curl_postfield_init(void)
{
    return (curl_postfield_t)calloc(1, sizeof (_curl_postfield_t));
}

string_t curl_postfield_serialize(curl_postfield_t pool)
{
    assert(pool);

    string_t s = string_init();
    string_t s2 = string_init();

    curl_keyvalue_t *ptr = pool->pool;
    for (int i = 0; ptr != NULL; i++)
    {
        if(i > 0)
        {
            string_strncat(s, "&", 1);
        }

        assert(ptr->key);
        assert(ptr->value);
        string_format(s2, "%s=%s", ptr->key, ptr->value);
        string_strncat(s, s2->ptr, s2->len);

        ptr = ptr->next;
    }
    string_destroy(s2);
    return s;
}

void curl_postfield_destroy(curl_postfield_t pool)
{
    assert(pool);

    curl_keyvalue_t *ptr = pool->pool;
    curl_keyvalue_t *prev = NULL;

    while (ptr != NULL) {
        prev = ptr;
        ptr = ptr->next;
        free(prev);
    }

    free(pool);
}
