/* 
 * grid.h - string (of x*y dimension) representation of map
 * 
 * Wenxin (Rose) Zhao, February 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "players.h"
#include "gold.h"


/**************** global types ****************/
typedef struct grid grid_t;

/**************** functions ****************/
int grid_getX(grid_t* grid);        // get horizontal length of grid
int grid_getY(grid_t* grid);        // get vertical dimension of grid
char* grid_getStr(grid_t* grid);    // get string representation of grid map

/**************** grid_new ****************/
/* Read the map file and create a grid
 * 
 * We return:
 *   pointer to a new grid, or NULL if error.
 * Caller is responsible for:
 *   later calling grid_delete.
 */
grid_t* grid_new(char* mapFile);

/**************** grid_get ****************/
/* Return the char at the given location n.
 *
 * Caller provides:
 *   valid integer n and valid string pointer to the current map.
 * We return:
 *   a char of what the location on the map has 
 *   \0 if map is NULL or n is larger than the map size or smaller than 0.
 * Notes:
 *   The char is *not* removed from the map.
 */
char grid_get(char* map, int n);


/**************** grid_hasSpace ****************/
/* Check if there is a '.' in the map
 * Returns:
    0 if no '.' left
    1 if there is
 */
int grid_hasSpace(char* map);

/**************** grid_randloc ****************/
/* Get a random location index that is '.' on the grid
 *
 * Caller provides:
 *   valid pointer to the grid
 *   integer seed
 * We return:
 *   an int of the location
 *   -1 if grid is NULL, or there is no '.' left in map
 */
int grid_randloc(char* map, int seed);

/**************** grid_NtoX ****************/
/* Convert linear coordinate of grid to x in x-y coordinate
 *
 * Caller provides:
 *   valid integer n and valid grid pointer
 * We return:
 *   -1 if grid is NULL or n is smaller than 0 or larger than grid size
 *   an int of the x coordinate
 * Note:
 *   n=0 starts at x,y=0,0; n=1 correspond to x=1,y=0
 */
int grid_NtoX(grid_t* grid, int n);

/**************** grid_NtoX ****************/
/* Convert linear coordinate of grid to y in x-y coordinate
 *
 * Caller provides:
 *   valid integer n and valid grid pointer
 * We return:
 *   -1 if grid is NULL or n is smaller than 0 or larger than grid size
 *   an int of the y coordinate
 */
int grid_NtoY(grid_t* grid, int n);/**************** grid_NtoX ****************/

/* Convert x-y coordinate of grid to linear coordinate
 *
 * Caller provides:
 *   valid integer x and y and valid grid pointer
 * We return:
 *   -1 if grid is NULL or x or y is smaller than 0 or larger than grid dimension
 *   an int of the n coordinate
 */
int grid_XYtoN(grid_t* grid, int x, int y);

/**************** grid_display ****************/
/* Display the grid to clients, taking the players, gold, map, and visibility 
 * into account.
 * 
 * Caller provides:
 *   valid grid pointer
 *   valid players pointer
 *   valid gold pointer
 *   player's character key (or ' ' for the spectator)
 *   valid visibility string pointer
 * We return:
 *   a pointer to a string map with visibility incorporated
 * User is responsible for:
 *   later freeing the returned string
 */
char* grid_display(grid_t* grid, players_t* players, gold_t* gold, 
                   char player, char* vis);

/**************** grid_iterate ****************/
/* Iterate over the grid, calling a function on each character.
 * 
 * Caller provides:
 *   valid grid pointer,
 *   arbitrary argument (pointer) that is passed-through to itemfunc,
 *   valid pointer to function that handles the pointer to the character.
 * We do:
 *   nothing, if grid==NULL or itemfunc==NULL.
 *   otherwise, call the itemfunc on each character, with (arg, char).
 * Notes:
 *   the order in which grid items are handled is undefined.
 *   the grid and its contents are not changed by this function,
 *   but the itemfunc may change the contents of the item.
 */
void grid_iterate(grid_t* grid, void* arg, void (*itemfunc)(void* arg, char* c));

/**************** grid_delete ****************/
/* Delete grid.
 *
 * Caller provides:
 *   valid grid pointer.
 * We do:
 *   if grid==NULL, do nothing.
 *   free the grid and gridStr.
 */
void grid_delete(grid_t* grid);

/**************** visibility ****************/
/* update visibility string based on player location
 *
 * Caller provides:
 *   valid grid pointer, visibility string pointer, valid player x-y coordinates
 * We do:
 *   change each char in vis string to:
 *      0 if not visible
 *      1 if currently visible
 *      2 if visible before but not now
 *   vis string unchanged if player location is not valid (i.e. not on a '.'/'#')
 * Note: 
 *   for the first call, the visibility string should be initialized as a string with all 0's and length=gridStr's length 
 */
char* visibility(grid_t* grid, char* vis, int px, int py);


