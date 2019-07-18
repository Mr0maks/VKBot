#include <pthread.h>
#include <memory.h>

#include "vkapi.h"

struct list {
  vkapi_message_new_object *object;
  struct list *next;
};
struct queue {
  struct list *head, *tail;
};

struct queue *queue_pool = NULL;

int queue_is_empty() {
  return (queue_pool->head == NULL && queue_pool->tail == NULL);
}

void add_queue(cJSON *update_block) {
  struct list *element = malloc(sizeof(struct list));
  vkapi_message_new_object *x = malloc(sizeof(vkapi_message_new_object));
  x->text = init_string();

  cJSON *object = cJSON_GetObjectItem(update_block, "object");
  cJSON *peer_id = cJSON_GetObjectItem(object, "peer_id");
  cJSON *from_id = cJSON_GetObjectItem(object, "from_id");

  x->peer_id = peer_id->valueint;
  x->from_id = from_id->valueint;

  cJSON *text_string = cJSON_GetObjectItem(object, "text");

  if(text_string)
    push_to_string(x->text, cJSON_GetStringValue(text_string));

  cJSON *attachments = cJSON_GetObjectItem(object, "attachments");

  x->attachmens = NULL;

  if(cJSON_GetArraySize(attachments) > 0)
    x->attachmens = cJSON_Duplicate(attachments, true);

  element->object = x;
  element->next = NULL;

  if (queue_pool->head == NULL) {
      queue_pool->head = element;
      queue_pool->tail = element;
    } else {
      struct list *old_tail = queue_pool->tail;
      old_tail->next = element;
      queue_pool->tail = element;
    }
}

void queue_init()
{
  queue_pool = malloc(sizeof(struct queue));
  queue_pool->head = NULL;
  queue_pool->tail = NULL;
}

vkapi_message_new_object *get_queue() {
  struct list *head = queue_pool->head;

  if(head == NULL)
    return NULL;

  queue_pool->head = head->next;
  vkapi_message_new_object *value = head->object;

  free(head);
  return(value);
}
