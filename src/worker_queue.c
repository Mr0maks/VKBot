#include <cJSON.h>
#include <pthread.h>
#include <memory.h>

struct list {
  cJSON *object;
  struct list *next;
};
struct queue {
  struct list *head, *tail;
};

struct queue *queue_pool = NULL;


int queue_is_empty() {
  return (queue_pool->head == NULL && queue_pool->tail == NULL);
}

void add_queue(cJSON *x) {
  struct list *element = malloc(sizeof(struct list));
  element->object = x;
  element->next = NULL;

  if (queue_pool->head == NULL) {
      queue_pool->head = element;
      queue_pool->tail = element;
    } else {
      struct list *oldTail = queue_pool->tail;
      oldTail->next = element;
      queue_pool->tail = element;
    }
}

void queue_init()
{
  queue_pool = malloc(sizeof(struct queue));
  queue_pool->head = NULL;
  queue_pool->tail = NULL;
}

void dbg_print() {
  struct list *h;
  int i = 0;

  if(queue_is_empty()) {
      printf("queue is empty!\n");
      return;
    }

  for(h = queue_pool->head; h!= NULL; h=h->next)
    i++;

  printf("In queqe have %i\n", i);
  return;
}

cJSON *get_queue() {
  struct list *head = queue_pool->head;

  if(head == NULL)
    return NULL;

  queue_pool->head = head->next;
  cJSON *value = head->object;

  free(head);
  return(value);
}
