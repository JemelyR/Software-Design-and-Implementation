/*
 * gold.h - header file for gold module
 *
 * The *gold* data structure is a nugget set wrapper. It adds functions to:
 *   - intialize with a random number of piles (pairs) with random quantities
 *   - get the amount of gold at any location on the game map
 *   - get a player's purse contents from the map string size and the player's
 *     character key
 *   - transfer gold from a location on the game map to a player's purse
 *
 * Rachel Matthew, February 2023
 */

#ifndef __GOLD_H
#define __GOLD_H

#include <stdbool.h>

/*************** global types ***************/
typedef struct gold gold_t;

/**************** functions ****************/

/**************** gold_new ****************/
/* Create a new set of gold-piles and store the given map string size. 
 *
 * We return:
 *   pointer to a new gold data struct, or NULL if error.
 * We guarantee:
 *   The gold is initialized with a random number from 'minPiles' to 'maxPiles' 
 *   piles of gold, the value of each combined summing up to 'total'.
 * Caller is responsible for:
 *   later calling gold_delete.
 */
gold_t* gold_new(const int total, const int minPiles, const int maxPiles, 
                 char* map, const int seed);

/**************** gold_get ****************/
/* Get the value of a gold-pile at a given location.
 *
 * We return:
 *   if there is a gold-pile in the set at the given location, return its
 *   integer value;
 *   otherwise, return 0
 */
int gold_get(gold_t* gold, const int loci);

/**************** gold_getPurse ****************/
/* Get the value in the purse of a given player.
 *
 * We return:
 *   if the player's purse is in the set of gold-piles, return its integer value
 *   otherwise, return 0
 */
int gold_getPurse(gold_t* gold, const char player);

/**************** gold_getRemaining ****************/
/* Get the amount of gold yet to be found.
 *
 * Caller provides:
 *   valid pointer to gold data structure
 * We return:
 *   the amount of gold initialized in the gold data structure that was yet to
 *   be found.
 *   IF gold is null, return -1.
 */
int gold_getRemaining(gold_t* gold);

/**************** gold_find ****************/
/* Transfer the gold at a given location to a given player's purse.
 *
 * We return:
 *   true if the transfer is a success, false otherwise.
 * We guarantee:
 *   the gold-pile at the given location is removed from the set of gold-piles
 *   its value is added to the contents of the player's purse
 * Caller is responsible for:
 *   handling a false return in case of error.
 */
bool gold_find(gold_t* gold, const char player, const int loci);

/*************** gold_addToMap ***************/
/* Add gold onto a map string
 *
 * We guarantee:
 *   For every gold-pile which is not a player's purse, adds a '*' to the map
 *   string at its location.
 *
 * no return value.
 */
void gold_addToMap(gold_t* gold, char* map);

/**************** gold_delete ****************/
/* Delete the gold data structure.
 *
 * Caller provides:
 *   valid gold pointer
 * We do:
 *   if gold==NULL, do nothing.
 *   otherwise, delete the set of gold-piles and free the gold.
 */
void gold_delete(gold_t* gold);

#endif // __GOLD_H
