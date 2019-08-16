#pragma once
#include "vkapi.h"
#include <cJSON.h>

vkapi_boolean vkapi_json_long_poll_have_updates( cJSON *main );
int vkapi_json_parse_groups_getById( string_t s );

