#include "common.h"
#include <string.h>

#define MEMORY_SENTINEL 0xDEADF00D

typedef struct memory_header_s
{
	uint32_t sentinel1;
	struct memory_header_s *next;
	size_t size;
	const char *filename;
	int line;
	uint32_t sentinel2;
} memory_header_t;

#define __calloc(x,y) __malloc(x*y)

#define PRINT_ERROR(x) Con_Printf("%s:%i: %s\n", file, line, x)

static memory_header_t *pool = NULL;
static size_t memory_used = 0;
static size_t memory_realy_used = 0;
static pthread_mutex_t malloc_mutex;

void *_malloc(size_t size, const char *file, int line)
{
	if(size == 0)
	{
		PRINT_ERROR("MEMORY: zero size to alloc");
		return NULL;
	}

	size_t size_need = (size + sizeof (memory_header_t));

	memory_header_t *ptr = malloc(size_need);

	ptr->next = pool;
	pool = ptr->next;

}


void _free(void *ptr, const char *file, int line)
{

}
