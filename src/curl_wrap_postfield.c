/*
curl_wrap_postfield.c - POST field for curl wrapper
Copyright (C) 2020  Mr0maks <mr.maks0443@gmail.com>

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.
*/

#include "common.h"

static curl_keyvalue_t* curl_keyvalue_new(const char* key, const char* value)
{
    curl_keyvalue_t* ptr = calloc(1, sizeof(curl_keyvalue_t));
    ptr->key = key;
    ptr->value = value;
    ptr->next = NULL;
    return ptr;
}

void curl_postfield_push(curl_postfield_t pool, const char* key, const char* value)
{
    assert(pool);
    assert(key);
    assert(value);

    curl_keyvalue_t* ptr = curl_keyvalue_new(key, value);

    ptr->next = pool->pool;
    pool->pool = ptr;
}

curl_postfield_t curl_postfield_init(void)
{
    return (curl_postfield_t)calloc(1, sizeof(_curl_postfield_t));
}

string_t curl_postfield_serialize(curl_postfield_t pool)
{
    assert(pool);

    string_t s = string_init();
    string_t s2 = string_init();

    curl_keyvalue_t* ptr = pool->pool;
    for (int i = 0; ptr != NULL; i++) {
        if (i > 0) {
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

    curl_keyvalue_t* ptr = pool->pool;
    curl_keyvalue_t* prev = NULL;

    while (ptr != NULL) {
        prev = ptr;
        ptr = ptr->next;
        free(prev);
    }

    free(pool);
}
