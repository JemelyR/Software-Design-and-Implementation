/* 
 * nuggset.c - 'nugget set' module
 *
 * see nuggset.h for more information.
 *
 * Rachel Matthew, February 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "nuggset.h"

/**************** local types ****************/
typedef struct node {
  int key;           // search key for this item
  void* item;        // pointer to data for this item
  struct node* next; // link to next node
} node_t;

/**************** global types ****************/
typedef struct nuggset {
  struct node* head; // head of the list of items
} nuggset_t;

/**************** functions ****************/
/*************** local functions ***************/
/* not visible outside this file */
node_t* node_new(const int key, void* item);

/**************** global functions ****************/
/**************** nuggset_new() ****************/
/* see nuggset.h for description */
nuggset_t* 
nuggset_new(void)
{
  nuggset_t* nuggset = malloc(sizeof(nuggset_t));

  if (nuggset != NULL) {
    nuggset->head = node_new(-1, NULL);
    if (nuggset->head != NULL) {
      return nuggset;
    }
  }
  // errors run off here
  fprintf(stderr, "\nError: memory allocation failed in nuggset_new");
  return NULL;
}

/*** node_new() ***/
/* Create a new nugget set node with the given key and item pointer.
 */
node_t*
node_new(const int key, void* item)
{
  node_t* node = malloc(sizeof(node_t));
  if (node != NULL) {
    node->key = key;
    node->item = item;
    node->next = NULL;
    return node;
  }
  // errors run off here
  return NULL;
}

/*************** nuggset_getHead() ***************/
/* see nuggset.h for a description */
char* 
nuggset_getHead(nuggset_t* nuggset)
{
  if (nuggset != NULL && nuggset->head != NULL) {
    if (nuggset->head->item == NULL) { // no meta information:
      return "";                       // return blank string
    } else {
      return nuggset->head->item; // otherwise: return meta information
    }
  }
  return NULL; // in case of argument error
}

/**************** nuggset_setHead() ****************/
/* see nuggset.h for description */
void 
nuggset_setHead(nuggset_t* nuggset, char* string)
{
  if (nuggset != NULL && nuggset->head != NULL && string != NULL) {
    // new string allocation
    char* mstring = malloc(strlen(string) + 1); // allocate space for new
    strcpy(mstring, string);                    // fill it w/ string

    // old string removal
    char* oldstring = nuggset->head->item;
    if (oldstring != NULL) { // free current info-string if necessary
      free(nuggset->head->item);
    }

    // set new string to meta information in head
    nuggset->head->item = mstring; 
  }
}

/**************** nuggset_set() ****************/
/* see nuggset.h for description */
void* 
nuggset_set(nuggset_t* nuggset, const int key, void* item, bool last)
{
  if (nuggset != NULL && nuggset->head != NULL) {
    node_t* node = nuggset->head;

    // modify mode (key already exists)
    if (nuggset_get(nuggset, key) != NULL) {
      while ((node = node->next) != NULL) {
        if (node->key == key) {
          void* olditem = node->item;
          node->item = item;
          return olditem;
        }
      }
    
    // add mode (key is new)
    } else {
      node_t* newnode = node_new(key, item);

      if (newnode != NULL) { // new node successfully created
        if (last) {
          while (node->next != NULL) {
            node = node->next;
          }
          node->next = newnode;

        } else {
          node_t* newnext = node->next;
          node->next = newnode;
          newnode->next = newnext;
        }
      } else { // in case of no memory to allocate a new node
        fprintf(stderr, "Error: memory allocation failed in nuggset_set");
      }
    }
  }
  return NULL;
}

/*************** nuggset_remove() ***************/
/* see nuggset.h for description */
bool 
nuggset_remove(nuggset_t* nuggset, const int key, void (*itemfunc)(void* item) )
{
  if (nuggset != NULL && nuggset->head != NULL) {
    node_t* node = nuggset->head;
    while (node->next != NULL) { // search for target npde
      if (node->next->key == key) { // find it:
        node_t* next = node->next;
        node->next = node->next->next; // 1) remove it from list
        if (itemfunc != NULL) {        // 2) free its item (if necessary)
          itemfunc(next->item);
        }
        free(next);                    // 3) free it.
        return true;
      }
      node = node->next;
    }
  }
  return false;
}

/**************** nuggset_get() ****************/
/* see nuggset.h for description */
void* 
nuggset_get(nuggset_t* nuggset, const int key)
{
  if (nuggset != NULL && nuggset->head != NULL) {
    node_t* node = nuggset->head;
    while ((node = node->next) != NULL ) {
      if (node->key == key) {
        return node->item;
      }
    }
  }
  return NULL;
}

/**************** nuggset_iterate() ****************/
/* see nuggset.h for description */
void 
nuggset_iterate(nuggset_t* nuggset, void* arg,
                void (*itemfunc)(void* arg, const int key, void* item) )
{
  if (nuggset != NULL && nuggset->head != NULL && itemfunc != NULL) {
    node_t* node = nuggset->head;
    while ((node = node->next) != NULL) {
      itemfunc(arg, node->key, node->item);
    }
  }
}

/**************** nuggset_delete() ****************/
/* see nuggset.h for description */
void 
nuggset_delete(nuggset_t* nuggset, void (*itemfunc)(void* item) )
{
  if (nuggset != NULL && nuggset->head != NULL) {
    node_t* node = nuggset->head;

    // free items
    while (node->next != NULL) {
      node_t* next = node->next;
      node->next = node->next->next;

      if (itemfunc != NULL) {
        itemfunc(next->item);
      }
      free(next);
    }

    // free head
    if (node->item != NULL) {
      free(node->item);
    }
    free(node);

    // free self
    free(nuggset);
  }
}


/* ****************************************************************** */
/* ************************* UNIT_TEST ****************************** */
/* 
 * This unit test creates, adds to, modifies, prints, and deletes a nugget set
 * to test the functionality of nugget set methods and check for memory leaks.
 *
 * The test is run with command line
 *   ./nuggtest
 * and should have no leaks if run with valgrind.
 */

#ifdef UNIT_TEST
#include <string.h>

void itemprint(void* arg, const int key, void* item);
void itemdelete(void* item);

int 
main()
{
  // Initial Print
  printf("TEST:\n");

  // Allocate space to items
  char* a = malloc(sizeof(char)*10);
  strcpy(a, "a item");
  char* b = malloc(sizeof(char)*10);
  strcpy(b, "basically");
  char* c = malloc(sizeof(char)*10);
  strcpy(c, "crows");
  char* d = malloc(sizeof(char)*10);
  strcpy(d, "direction");
  char* e = malloc(sizeof(char)*10);
  strcpy(e, "eventual");

  // Create nugget set
  nuggset_t* ns = nuggset_new();

  // Add one pair and print
  nuggset_set(ns, 1, a, false);
  nuggset_iterate(ns, NULL, itemprint);
  printf("\n");

  // Replace one pair, add three more, and print
  char* s = nuggset_set(ns, 1, c, false);
  free(s);
  nuggset_set(ns, 2, e, false);
  nuggset_set(ns, 6, b, false);
  nuggset_set(ns, 5, d, true);
  nuggset_iterate(ns, NULL, itemprint);
  printf("\n");

  // Remove one pair and print
  nuggset_remove(ns, 1, itemdelete);
  nuggset_iterate(ns, NULL, itemprint);
  printf("\n");

  // Delete nugget set
  nuggset_delete(ns, itemdelete);
  
  return 0;
}

/*** itemprint ***/
/* Helper Function "itemfunc" to nuggset_iterate. Prints each key-item pair
 * given to it to stdout.
 *
 * NOTE: Assumes item is a string.
 */
void itemprint(void* arg, const int key, void* item)
{
  char* i = item;
  printf(" %d:%s,", key, i);
}

/*** itemdelete ***/
/* Helper Function "itemfunc" to nuggset_iterate. Frees each item given to it.*/
void itemdelete(void* item) 
{
  if (item != NULL) {
    free(item);
  }
}

#endif // UNIT_TEST
