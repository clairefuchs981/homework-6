#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Note: this is just my code from hw1, so you can ignore. */

/* Data type for node in a doubly linked list */
typedef struct List_element{
  void    *data;                /* data blob can be any type  */ 
  struct  List_element *next;   /* pointer to the next node  */ 
  struct  List_element *prev;   /* pointer to the previous node  */ 
} List_element;

/* Data type for doubly linked list  */
typedef struct List{
  List_element *head;                                     /* node pointer to head of list */ 
  List_element *tail;                                     /* node pointer to tail of list */
  int          size;                                      /* number of nodes in list */ 
  char*        (*tostr)(void *data);                      /* function pointer to print a node in the list */
  int          (*destroy)(void *data);                    /* function pointer to free a list node */
  int          (*is_equal)(void *target, void *current);  /* function pointer to tell if two list elements are equal */  
} List;

List * list_create(char* (*tostr)(void *data),
                   int (*des)(void *data),
                   int (*is_equal)(void *target, void *current)){
  List *list = malloc(sizeof(List));
  list->head = NULL;
  list->tail = NULL;
  list->size = 0;
  list->tostr = tostr;
  list->destroy = des;
  list->is_equal = is_equal; 
  return list;
}

int list_get_size(List *list) {
  return list->size;
}

void list_append(List *list, void *val){
  List_element *curr, *prev;
  prev = list->tail;
  
  curr   = malloc(sizeof(List_element));
  curr->data = val; 

  curr->next = NULL;
  list->tail = curr;

  if (list->head == NULL){
    list->head = curr;
    curr->prev = NULL;
  }
  else{
    curr->prev = prev;
    prev->next = curr;
  }
 
  ++list->size;
}

void list_prepend(List *list, void *val){
  List_element *curr, *next;
  next = list->head;
 
  curr  = malloc(sizeof(List_element));
  curr->data = val;

  list->head = curr;
  curr->prev = NULL;
  curr->next = next;

  if (next == NULL) { /* prepended node is only node in list */ 
    list->tail = curr;
  }

  ++list->size; 
}

void list_print(List *list){
  List_element *node = list->head; 
  printf("[\n"); 
  while (node != NULL) {
    printf("%s\n", list->tostr(node->data));
    node = node->next; 
  }
  printf("]\n"); 
}

void list_insert(List  *list, int index, void *val){
  List_element *node = list->head, *new_node;
  int count = 0;

  /* error checking; put value on appropriate end of list if index fires off in either direction */ 
  if (index >= list->size){
    printf("Index value %d is at end of list or beyond: appending value.\n", index);
    list_append(list, val);
    return;
  } else if (index <= 0) {
    printf("Index value %d is 0 or negative; prepending value.\n", index);
    list_prepend(list, val); 
    return;  
  }

  /** iterate over list until you find the insert place, then wedge a new node in */ 
  while (count != index) {
    node = node->next; 
    count++;   
  }

  new_node = malloc(sizeof(List_element));
  new_node->data = val;

  new_node->prev = node; 
  new_node->next = node->next;
  node->next = new_node; 
    
  ++list->size; 
}

void list_pop_front(List * list) {
  List_element *node = list->head, *curr; 

  if (node->next != NULL) {
    curr = node->next; 
    curr->prev = NULL; 
    list->head = curr;
  } else { /* remove node from a single node list */ 
    list->head = NULL; 
    list->tail = NULL; 
  }
  --list->size; 
  list->destroy(node->data);
  free(node->data);
  free(node); 
}

void list_pop_end(List * list) {
  List_element *node = list->tail, *curr;

  if (node->prev != NULL) {
    curr = node -> prev; 
    curr->next = NULL; 
    list->tail = curr;
  } else { /* remove a node from a single node list */ 
    list->head = NULL; 
    list->tail = NULL;
  }
  --list->size; 
  list->destroy(node->data); 
  free(node->data); 
  free(node); 
}

void list_remove(List  *list, int index){
  List_element *node = list->head;
  int count;

  if (index < 0 || index > (list->size - 1)) {
    printf("Invalid index %d for list_remove. Returning without removing.", index);
    return;
  } 

  /* handle edges */ 
  if (index == 0) {
    printf("Index is %d, popping front element of list.", index); 
    list_pop_front(list);
    return;
  } else if (index == list->size - 1) {
    printf("Index is %d, popping off end of list.", index); 
    list_pop_end(list);
    return; 
  }

  while (count < index) {
    node = node->next;
    count++;
  }

  node->prev->next = node->next;
  node->next->prev = node->prev;
 
  --list->size;
  list->destroy(node->data); 
  free(node->data);
  free(node);
}

void* list_element_at(List *list, int index){
  List_element *node = list->head;
  int count = 0;

  if (index < 0 || index > list->size - 1) {
    printf("Invalid index %d passed to list_element_at.", index);
    return NULL; 
  }
  
  for (count = 0; count < list->size; count++) {
    if (count == index) {
      return node->data;
    }
    node = node->next;
  }
  return NULL; 
}

void list_clear(List *list){
  List_element *node = list->head, *next;
  
  while (node != NULL) {
    next = node->next; 
    list->destroy(node->data);
    free(node);
    node = next;
  }
  
  list->head = NULL;
  list->tail = list->head;
}


int list_contains(List * list, void *data) {
  List_element *node = list->head; 
  
  while (node != NULL) {
    if (list->is_equal(node->data, data)) {
      return 1; 
    }
    node = node->next; 
  }
  return 0; 
}


int list_find_first(List *list, void* data){
  List_element *node = list->head; 
  int index; 

  for (index = 0; index < list->size; index++) {
    if (list->is_equal(node->data, data)) {
      return index; 
    }
    node = node->next; 
  }
  return -1;   
}
