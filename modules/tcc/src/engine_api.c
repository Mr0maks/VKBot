#include <cJSON.h>
#include <string.h>
#include <module_api.h>
#include <enginecallbacks.h>
#include <libtcc.h>
#include "va_utils.h"

engine_api_t engine_api;

void vk_tcc_error_func( void *data, const char *str )
{
	VKAPI_SEND_MESSAGE(((vkapi_message_object*)data)->peer_id, str, NULL, 0);
}

void cmd_tcc ( vkapi_message_object *message, int argc, char **argv, const char *args )
{
	if(argc)
	{
		usage:
		VKAPI_SEND_MESSAGE(message->peer_id, "Использование: tcc <приклиплённый файл>", NULL, 0);
		return;
	}

	if(!message->attachmens)
		goto usage;

	cJSON *doc = cJSON_GetObjectItem(cJSON_GetArrayItem(message->attachmens, 0), "doc");

	if(!doc)
	{
		goto usage;
	}

	string_t file = STRING_INIT();
	string_t url = STRING_INIT();

	STRING_COPY(url, cJSON_GetStringValue(cJSON_GetObjectItem(doc, "url")));

	CURL_GET(NULL, url, NULL, file);

	TCCState *state = tcc_new();
	tcc_set_error_func(state, message, vk_tcc_error_func);

	tcc_add_file(state, "/usr/lib64/libc.a");
	tcc_add_library_path(state, "/usr/lib64/tcc");
	tcc_add_sysinclude_path(state, "/usr/include");
	tcc_add_sysinclude_path(state, "/usr/include/linux");
	tcc_set_output_type(state, TCC_OUTPUT_MEMORY);

	if(tcc_compile_string(state, file->ptr) == -1)
	{
	VKAPI_SEND_MESSAGE(message->peer_id, "Ошибка при компиляции", NULL, 0);
	tcc_delete(state);
	STRING_DESTROY(file);
	STRING_DESTROY(url);
	return;
	}

	int result = tcc_run(state, 0, NULL);

	VKAPI_SEND_MESSAGE(message->peer_id, va("Программа вернула значение: %i", result), NULL, 0);

	tcc_delete(state);
	STRING_DESTROY(file);
	STRING_DESTROY(url);
}

module_info_t module_info =
{
	"tcc",
	"0.0.1",
	__DATE__,
	"https://github.com/Mr0maks",
	"Mr0maks",
	ENGINE_API_VERSION
};

void Module_Init(int apiver, module_info_t **info, engine_api_t *apifuncs)
{
	memcpy(&engine_api, apifuncs, sizeof(engine_api));
	*info = &module_info;

	REGISTER_COMMAND(&module_info, "tcc", "tcc", cmd_tcc);
}
