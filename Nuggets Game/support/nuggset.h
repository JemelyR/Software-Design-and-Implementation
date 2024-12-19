/* 
 * nuggset.h - header file for nugget set module
 *
 * A *nugget set* maintains an unordered collection of (key,item) pairs; 
 * any given key can only occur in the set once. 
 *
 * It starts out empty and grows as the caller adds new (key,item) pairs. The 
 * caller can retrieve, remove, and update items by using their key. Items are 
 * distinguished by their integer key.
 *
 * The head of a nugget set is a specialized node containing a string of meta 
 * information about the set. It is not considered an item in the set.
 *
 * Rachel Matthew, February 2023
 */

#ifndef __NUGGSET_H
#define __NUGGSET_H

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "nuggset.h"

/**************** global types ****************/
typedef struct nuggset nuggset_t;  // opaque to users of the module

/**************** functions ****************/

/**************** nuggset_new ****************/
/* Create a new (empty) nugget set.
 * 
 * We return:
 *   pointer to a new nugget set, or NULL if error.
 * We guarantee:
 *   The nugget set is initialized empty.
 * Caller is responsible for:
 *   later calling nuggset_delete.
 */
nuggset_t* nuggset_new(void);

/**************** nuggset_getHead ****************/
/* Get the meta information string at the head of a nugget set.
 * 
 * Caller provides:
 *   valid nugget set pointer
 * We return:
 *   The meta information string, or NULL if error.
 * Caller is responsible for:
 *   1. NOT freeing the returned string, as a later call to nuggset_delete or
 *      nuggset_setHead will take care of that and be disrupted by this action.
 *   2. NOT attempting to set the head of the nugget set as this functions's
 *      return value. Doing so may not result in any issues in functionality, 
 *      but it will cause a valgrind-detectable memory error.
 */
char* nuggset_getHead(nuggset_t* nuggset);

/**************** nuggset_setHead ****************/
/* Set the meta information at the head of a nugget set to a given string.
 * 
 * Caller provides:
 *   valid nugget set pointer and non-null string
 * We guarantee:
 *   The previous meta information string is freed and space in the nugget set's
 *   head is allocated and used to store the new given string.
 * Caller is responsible for:
 *   NOT giving the current nugget set head's value as an argument
 *
 * no return value.
 */
void nuggset_setHead(nuggset_t* nuggset, char* string);

/**************** nuggset_set ****************/
/* Set an item, identified by a key (integer), in a nugget set.
 *
 * Caller provides:
 *   valid nugget set pointer, integer key, pointer to item, and 'last' boolean.
 * We return:
 *   a pointer to the previous item if the key already existed in the nugget set
 *   NULL otherwise
 * Caller is responsible for:
 *   freeing the resulting pointer if it is not null and no longer needed
 * Notes:
 *   If the key is not present in the nugget set, the (key, ite) pair is added:
 *      just after the head if last is false
 *      at the tail of the nugget set is last is true
 *   If the key is present, then its item is simply modified. 
 *   IMPORTANT: No frees are performed in this process; thus, the caller is 
 *   responsible for being aware of what they modify in a nugget set.
 */
void* nuggset_set(nuggset_t* nuggset, const int key, void* item, bool last);

/*************** nuggset_remove ***************/
/* Remove an item identified by a key (integer) from a nugget set and call a
 * delete function on it.
 *
 * Caller provides:
 *   valid nugget set pointer
 *   integer key
 *   valid pointer to function that handles one item (may be NULL)
 * We return:
 *   true if the remove is successful
 *   false otherwise
 * Caller is responsible for:
 *   checking for a false return and handling it accordingly
 */
bool nuggset_remove(nuggset_t* nuggset, const int key, 
                    void (*itemfunc)(void* item) );

/**************** nuggset_get ****************/
/* Return the item associated with the given key.
 *
 * Caller provides:
 *   valid nugget set pointer, integer key.
 * We return:
 *   a pointer to the desired item, if found; 
 *   NULL if nugget set is NULL or key is not found.
 * Notes:
 *   The item is *not* removed from the nugget set.
 *   Thus, the caller should *not* free the pointer that is returned.
 */
void* nuggset_get(nuggset_t* nuggset, const int key);

/**************** nuggset_iterate ****************/
/* Iterate over the nugget set, calling a function on each item.
 * 
 * Caller provides:
 *   valid nugget set pointer,
 *   arbitrary argument (pointer) that is passed-through to itemfunc,
 *   valid pointer to function that handles one item.
 * We do:
 *   nothing, if nuggset==NULL or itemfunc==NULL.
 *   otherwise, call the itemfunc on each item, with (arg, key, item).
 * Notes:
 *   the order in which nugget set items are handled is undefined.
 *   the nugget set and its contents are not changed by this function,
 *   but the itemfunc may change the contents of the item.
 */
void nuggset_iterate(nuggset_t* nuggset, void* arg,
                 void (*itemfunc)(void* arg, const int key, void* item) );

/**************** nuggset_delete ****************/
/* Delete nugget set, calling a delete function on each item.
 *
 * Caller provides:
 *   valid nugget set pointer,
 *   valid pointer to function that handles one item (may be NULL).
 * We do:
 *   if nuggset==NULL, do nothing.
 *   otherwise, unless itemfunc==NULL, call the itemfunc on each item.
 *   free the set itself.
 */
void nuggset_delete(nuggset_t* nuggset, void (*itemdelete)(void* item) );

#endif // __NUGGSET_H
