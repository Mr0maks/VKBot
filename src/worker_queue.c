#include "common.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct list_s {
  vkapi_message_object *object;
  struct list_s *next;
} list_t;

struct queue {
  list_t *head, *tail;
};

static struct queue *queue_pool = NULL;
static volatile size_t maxium_tasks_in_queue;
static volatile size_t queue_tasks_in_queue;

int queue_empty() {
  return (queue_pool->head == NULL && queue_pool->tail == NULL);
}

size_t queue_maxium_tasks()
{
  return maxium_tasks_in_queue;
}

void queue_push(cJSON *update_block) {
  cJSON *object = cJSON_GetObjectItem(update_block, "object");
  cJSON *peer_id = cJSON_GetObjectItem(object, "peer_id");
  cJSON *from_id = cJSON_GetObjectItem(object, "from_id");

  if(!object || !peer_id || !from_id) {
    cJSON_Delete(update_block);
    return;
    } else if(peer_id->valueint < 0 || from_id->valueint < 0)
    {
      cJSON_Delete(update_block);
      return;
    }

  list_t *element = malloc(sizeof(list_t));
  vkapi_message_object *x = calloc(1, sizeof(vkapi_message_object));

  x->from_id = from_id->valueint;
  x->peer_id = peer_id->valueint;

  x->private_message = false;

  if(x->peer_id == x->from_id)
    x->private_message = true;

  x->text = string_init();

  cJSON *text_string = cJSON_GetObjectItem(object, "text");

  if(text_string)
    string_copy(x->text, cJSON_GetStringValue(text_string));

  cJSON *attachments = cJSON_GetObjectItem(object, "attachments");

  x->attachmens = NULL;

  if(cJSON_GetArraySize(attachments) > 0)
    x->attachmens = cJSON_Duplicate(attachments, true);

  cJSON_Delete(update_block);

  queue_tasks_in_queue++;

  maxium_tasks_in_queue = MAX(queue_tasks_in_queue, maxium_tasks_in_queue);

  element->object = x;
  element->next = NULL;

  if (queue_pool->head == NULL) {
      queue_pool->head = element;
      queue_pool->tail = element;
    } else {
      list_t *old_tail = queue_pool->tail;
      old_tail->next = element;
      queue_pool->tail = element;
    }
}

vkapi_message_object *queue_pop() {
  list_t *head = queue_pool->head;

  if(head == NULL)
    return NULL;

  queue_tasks_in_queue--;

  queue_pool->head = head->next;
  vkapi_message_object *value = head->object;

  free(head);
  return(value);
}

void queue_init()
{
  queue_pool = GC_MALLOC_UNCOLLECTABLE(sizeof(struct queue));
  queue_tasks_in_queue = 0;
  maxium_tasks_in_queue = 0;
  queue_pool->head = NULL;
  queue_pool->tail = NULL;
}

void queue_deinit()
{
  free(queue_pool);
}
