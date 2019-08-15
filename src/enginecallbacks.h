#ifndef ENGINECALLBACKS_H
#define ENGINECALLBACKS_H

#include <module_api.h>

extern engine_api_t engine_api;

#define string_init (*engine_api.string_init)
#define string_dublicate (*engine_api.string_dublicate)
#define string_format (*engine_api.string_format)
#define string_strncat (*engine_api.string_strncat)
#define string_copy (*engine_api.string_copy)
#define string_destroy (*engine_api.string_destroy)


#endif // ENGINECALLBACKS_H
