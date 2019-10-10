#include "common.h"

#define MAX(a,b) ((a) > (b) ? (a) : (b))

typedef struct list_s {
  cJSON *object;
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
  list_t *element = calloc(1, sizeof (list_t));

  element->object = update_block;

  queue_tasks_in_queue++;

  maxium_tasks_in_queue = MAX(queue_tasks_in_queue, maxium_tasks_in_queue);

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

cJSON *queue_pop(void) {
  list_t *head = queue_pool->head;

  if(head == NULL)
    return NULL;

  queue_tasks_in_queue--;

  queue_pool->head = head->next;
  cJSON *value = head->object;

  free(head);
  return(value);
}

void queue_init(void)
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
