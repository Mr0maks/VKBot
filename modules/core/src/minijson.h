#pragma once

#include <stdbool.h>
#include <stdlib.h>
#include <strings.h>
#include <string.h>

enum
{
	MINI_JSON_INVALID = 0,
	MINI_JSON_BOOL,
	MINI_JSON_NULL,
	MINI_JSON_NUMBER,
	MINI_JSON_STRING,
	MINI_JSON_ARRAY,
	MINI_JSON_OBJECT,
	MINI_JSON_RAW
};

/* The cJSON structure: */
typedef struct minijson
{
	struct minijson *next;
	struct minijson *prev;
	/* An array or object item will have a child pointer pointing to a chain of the items in the array/object. */
	struct minijson *child;

	char *string;
	char *valuestring;

	int type;
	int valueint;

	double valuedouble;
	bool valuebool;
} minijson;

static minijson *get_object_item(const minijson * const object, const char * const name, const bool case_sensitive)
{
	minijson *current_element = NULL;

	if ((object == NULL) || (name == NULL))
	{
		return NULL;
	}

	current_element = object->child;
	if (case_sensitive)
	{
		while ((current_element != NULL) && (current_element->string != NULL) && (strcmp(name, current_element->string) != 0))
		{
			current_element = current_element->next;
		}
	}
	else
	{
		while ((current_element != NULL) && (strcasecmp(name, current_element->string) != 0))
		{
			current_element = current_element->next;
		}
	}

	if ((current_element == NULL) || (current_element->string == NULL)) {
		return NULL;
	}

	return current_element;
}

static minijson *minijson_getobjectitem(const minijson * const object, const char * const string)
{
	return get_object_item(object, string, false);
}

static minijson *minijson_getobjectitemcasesensitive(const minijson * const object, const char * const string)
{
	return get_object_item(object, string, true);
}


static minijson* get_array_item(const minijson *array, size_t index)
{
	minijson *current_child = NULL;

	if (array == NULL)
	{
		return NULL;
	}

	current_child = array->child;
	while ((current_child != NULL) && (index > 0))
	{
		index--;
		current_child = current_child->next;
	}

	return current_child;
}

static minijson *minijson_getarrayitem(const minijson *array, int index)
{
	if (index < 0)
	{
		return NULL;
	}

	return get_array_item(array, (size_t)index);
}

static int minijson_getarraysize(const minijson *array)
{
	minijson *child = NULL;
	size_t size = 0;

	if (array == NULL)
	{
		return 0;
	}

	child = array->child;

	while(child != NULL)
	{
		size++;
		child = child->next;
	}

	/* FIXME: Can overflow here. Cannot be fixed without breaking the API */

	return (int)size;
}

static char *minijson_getstringvalue(minijson * object)
{
	if(object == NULL) return NULL;
	if(object->valuestring != NULL) return object->valuestring;
}

static void minijson_delete(minijson *item)
{
	minijson *next = NULL;
	while (item != NULL)
	{
		next = item->next;
		if (item->child != NULL)
		{
			minijson_delete(item->child);
		}
		if (item->valuestring != NULL)
		{
			free(item->valuestring);
		}
		if (item->string != NULL)
		{
			free(item->string);
		}
		free(item);
		item = next;
	}
}
#define minijson_arrayforeach(element, array) for(element = (array != NULL) ? (array)->child : NULL; element != NULL; element = element->next)
minijson *minijson_parse(const char *string);
