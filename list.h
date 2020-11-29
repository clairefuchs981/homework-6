
#ifndef LIST_H
#define LIST_H
/* Note: this is just my code from hw1, so you can ignore. */

/* Data type for node in a doubly linked list */
typedef struct List_element{
  void    *data;                /* data blob for node; can be any type */ 
  struct  List_element *next;   /* pointer to next node or null */ 
  struct  List_element *prev;   /* pointer to previous node or null */ 
} List_element;

/* Data type for doubly linked list  */
typedef struct List{
  List_element *head;                                     /* node pointer to head of list or null */ 
  List_element *tail;                                     /* node pointer to tail of list or null */
  int          size;                                      /* number of nodes in list */ 
  char*        (*tostr)(void *data);                      /* function pointer to print a node in the list */
  int          (*destroy)(void *data);                    /* function pointer to free a list node */
  int          (*is_equal)(void *target, void *current);  /* function pointer to tell if two list elements are equal */  
} List;

/* Constructor for a list; requires user to provide destroy, tostr, and is_equal functions for their data type.  */ 
List * list_create(char* (*tostr)(void *data), int (*des)(void *data), int (*is_equal)(void *target, void *current));
/* List size getter  */
int list_get_size(List *list); 
/* Function to append a node to the end of the list. Assumes user has malloc'ed any memory needed for *data's internal structure already.  */ 
void list_append(List *list, void *val);
/* Function to append a node to the beginning of the list. Assumes user has malloc'ed any memory needed for *val's internal structure already.  */
void list_prepend(List *list, void *val);
/* Iterates over the list and prints all nodes using the user-defined tostr function */ 
void list_print(List *list);  
/* Inserts node at specified index in the list. Prepends or appends the value if the integer overflows in either direction. */
void list_insert(List *list, int index, void *val); 
/* Remove element at the start of the list. Uses user-defined destroy function.  */
void list_pop_front(List * list);
/* Remove element at the end of the list. Uses user-defined destroy function.  */
void list_pop_end(List * list);
/* Remove node at index, if index is valid. Uses user-defined destroy function. */
void list_remove(List *list, int index); 
/* Returns the element at the given index, or nothing if index is invalid. */ 
void* list_element_at(List *list, int index); 
/* Clears the list of all nodes. Also uses user-defined destroy function for nested data, if it exists. */ 
void list_clear(List *list);
/* Check if the data exists in the list, returns -1 if it does not.  */ 
int list_contains(List * list, void *data);   
/* Return index of the first occurence of the data, or -1 if it does not exist. */
int list_find_first(List *list, void* data); 
#endif