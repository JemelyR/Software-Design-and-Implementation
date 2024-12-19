/* 
 * gold.c - 'gold' module
 *
 * see gold.h for more information.
 *
 * Rachel Matthew, February 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "nuggset.h"
#include "grid.h"
#include "gold.h"

/*************** global types ***************/
typedef struct gold {
  struct nuggset* list; // list of gold-piles (and purses)
  int mapSize;          // map string length
  int remaining;        // remaining gold to be found
} gold_t;

/**************** functions ****************/
/**************** LOCAL FUNCTIONS ***************/
static void addToMap(void* arg, const int loci, void* value);
static void itemdelete(void* item);

/**************** GLOBAL FUNCTIONS ***************/
/**************** gold_new() ****************/
/* see gold.h for description */
gold_t* 
gold_new(const int total, const int minPiles, const int maxPiles, 
         char* map, const int seed)
{
  // Check constants for validity
  if (total < 0 || minPiles < 0 || maxPiles < 0) {
    fprintf(stderr, "Error: gold pile number and value must be nonnegative.");
  }

  // If valid:
  gold_t* gold = malloc(sizeof(gold_t));
  if (gold != NULL) { // check gold in memory
    gold->mapSize = strlen(map);
    gold->remaining = total;
    gold->list = nuggset_new();

    if (gold->list != NULL) { // check nugget set in memory
      srand(seed);
      // FILL nugget set with gold-piles
      // 1) Get number of gold-piles
      int piles = minPiles + rand() % (1 + maxPiles - minPiles); 

      // 2) Assign randomly-distributed values to each gold-pile
      int values[piles];
      for (int i = 0; i < piles; i++) { // initialize
        values[i] = 0;
      }
      for (int i = 0; i < total; i++) { // fill
        int j = rand() % piles;
        values[j]++;
      }

      // 3) Add piles to nugget set at random location
      for (int i = 0; i < piles; i++) {
        // generate location
        int locus = grid_randloc(map, rand());
        if (locus == -1) { // ran out of space; terminate
          gold_delete(gold);
          return NULL;
        }
        while (gold_get(gold, locus) != 0) {
          locus = grid_randloc(map, rand());
        }
        // add to set
        int* valuePointer = malloc(sizeof(int));
        *valuePointer = values[i];
        nuggset_set(gold->list, locus, valuePointer, false);
      }
      return gold;

    } else {
      free(gold);
    }
  }
  // allocation errors run off here
  fprintf(stderr, "Error: memory allocation in gold_new failed");
  return NULL;
}

/**************** gold_get() ****************/
/* see gold.h for description */
int 
gold_get(gold_t* gold, const int loci)
{
  int value = 0;

  if (gold != NULL && gold->list != NULL && loci < gold->mapSize) {
    int* valuePointer = nuggset_get(gold->list, loci);
    if (valuePointer != NULL) {
      value = *valuePointer;
    }
  }
  return value;
}

/**************** gold_getPurse() ****************/
/* see gold.h for description */
int 
gold_getPurse(gold_t* gold, const char player)
{
  int value = 0;
  if (gold != NULL && gold->list != NULL) {
    int purseLoci = gold->mapSize + 1 + player - 'A';
    int* valuePointer = nuggset_get(gold->list, purseLoci);
    if (valuePointer != NULL) {
      value = *valuePointer;
    }
  }

  return value;
}

/*************** gold_getRemaining() ***************/
/* see gold.h for description */
int 
gold_getRemaining(gold_t* gold)
{
  if (gold != NULL) {
    return gold->remaining;
  }
  return -1;
}

/**************** gold_find() ****************/
/* see gold.h for description */
bool 
gold_find(gold_t* gold, const char player, const int loci)
{
  if (gold != NULL && gold->list != NULL) {
    // compute player's new amount
    int newValue = gold_get(gold, loci) + gold_getPurse(gold, player);
    // assign player's new amount
    int* newValuePointer = malloc(sizeof(int));       // prepare new value
    *newValuePointer = newValue;                
    int purseLoci = gold->mapSize + 1 + player - 'A'; // get loci
    int* old = nuggset_set(gold->list, purseLoci, newValuePointer, true); 
    if (old != NULL) { free(old); } // free previous value if necessary

    // remove gold-pile value from remaining
    gold->remaining = gold->remaining - gold_get(gold, loci);
    // remove gold-pile from list and check that current values are correct
    return ((gold_getPurse(gold, player) == newValue) && 
            nuggset_remove(gold->list, loci, itemdelete));
  }
  return false;
}

/*************** gold_addToMap() ***************/
/* see gold.h for description */
void gold_addToMap(gold_t* gold, char* map) {
  if (gold != NULL && gold->list != NULL && map != NULL) {
    nuggset_iterate(gold->list, map, addToMap);
  }
}

/*** addToMap ***/
/* Helper Function to gold_addToMap: sets a gold-pile's location on a map string
 * to '*' if the gold-pile is located on the map-string.
 *
 * no return value.
 */
static void 
addToMap(void* arg, const int loci, void* value) 
{
  // properly cast arguments
  char* map = arg;
  if (map != NULL && loci < strlen(map)) {
    map[loci] = '*';
  }
}

/**************** gold_delete() ****************/
/* see gold.h for description */
void 
gold_delete(gold_t* gold)
{
  if (gold != NULL) {
    if (gold->list != NULL) {
      nuggset_delete(gold->list, itemdelete);
    }
    free(gold);
  }
}

/*** itemdelete ***/
/* Frees an item */
static void
itemdelete(void* item) 
{
  if (item != NULL) {
    free(item);
  }
}


/* ****************************************************************** */
/* ************************* UNIT_TEST ****************************** */
/* 
 * This unit test creates, modifies, prints, and deletes a gold set to test the 
 * functionality of gold methods and check for memory leaks.
 *
 * The test is run with command line
 *   ./goldtest
 * and should have no leaks if run with valgrind.
 */

#ifdef UNIT_TEST

#include <time.h>

int
main()
{
  char* map = malloc(19);
  strcpy(map, ".....\n.....\n.....\n");
  gold_t* gold = gold_new(500, 2, 10, map, time(0));

  printf("Print w/out gold...\n");
  puts(map); fflush(stdout);

  gold_addToMap(gold, map);
  printf("Printing with gold...\n");
  puts(map); fflush(stdout);

  for (int i = 0; i < strlen(map); i++) {
    if (map[i] == '*') {
      printf("%d - $%d\n", i, gold_get(gold, i));
    }
  }

  gold_delete(gold);
  free(map);
}

#endif // UNIT_TEST
