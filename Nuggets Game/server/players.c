/* 
 * players.c - 'players' module
 *
 * see players.h for more information.
 *
 * Rachel Matthew, February 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "nuggset.h"
#include "grid.h"
#include "players.h"

/**************** local types ****************/
typedef struct player {
  char key;          // alphabet key
  char* name;        // full name string
  char* visib;       // visibility string
  int loci;          // location in map string
} player_t;

/**************** global types ****************/
typedef struct players {
  struct nuggset* list;
} players_t;

/**************** functions ****************/
/*************** local functions ***************/
/* not visible outside this file */
static player_t* player_new(const char key, char* name, char* map);
static player_t* players_get(players_t* players, const char key);
static char updateHead(nuggset_t* list);
static void addToMap(void* arg, const int listKey, void* playerPointer);
static void playerdelete(void* playerPointer);

/**************** global functions ****************/
/**************** players_new() ****************/
/* see players.h for description */
players_t* 
players_new(void)
{
  players_t* players = malloc(sizeof(players_t));
  if (players != NULL) {
    players->list = nuggset_new();
    if (players->list != NULL) {
      nuggset_setHead(players->list, "A\0");
      return players;
    }
  }
  // errors run off here
  fprintf(stderr, "Error: memory allocation failed in players_new\n");
  return NULL;
}

/*************** players_getLast ***************/
/* see players.h for description */
char 
players_getLast(players_t* players)
{
  if (players != NULL && players->list != NULL) {
    char* mstring = nuggset_getHead(players->list);
    return mstring[0] - 1;
  }
  return 'A' - 1;
}

/**************** players_add() ****************/
/* see players.h for description */
char
players_add(players_t* players, char* name, char* map)
{
  if (players != NULL && players->list != NULL && name != NULL && map != NULL) {
    char key = updateHead(players->list); // get key from meta information
    if (key > 'Z') { // exit if invalid
      return 'Z' + 1;
    }

    // Otherwise:
    player_t* player = player_new(key, name, map);
    if (player != NULL) {
      if (player->loci == -1) { // give false return for players who couldn't be
        playerdelete(player);   // fit onto the grid map
        char* mstring = nuggset_getHead(players->list);
        mstring[0] = mstring[0] - 1;
        return 'Z' + 1;
      }
      int listKey = (int) key;
      nuggset_set(players->list, listKey, player, true);
      return key;
    }
  } else {
    fprintf(stderr, "Error: invalid arguments in players_add\n");
  }
  // errors run off here
  return 'Z' + 1;
}

/*** updateHead ***/
/* Return the character key stored at the start of the nugget set's meta 
 * information and increment it, unless it is already after 'Z'.
 *
 * Caller provides:
 *   a valid nugget set pointer
 * We return:
 *   the meta information-provided character key
 * We guarantee:
 *   the meta information character key will be increments UNLESS it is already
 *   after (i.e. greater than) 'Z'
 *
 * Invalid keys are handled in players_add, where this local function is used.
 */
static char 
updateHead(nuggset_t* list)
{
  char* metastring = nuggset_getHead(list);
  // if NULL, just return an invalid key
  if (metastring == NULL) {
    return ('Z' + 1);
  }
  // otherwise get actual key.
  char key = metastring[0];
  // check and potentially increment the string
  if (key <= 'Z') {
    metastring[0] = key + 1;
  }
  // return
  return key;
}

/*** player_new() ***/
/* Create a new player.
 *
 * Caller provides:
 *   a character key, a non-null player name string, and a non-null map string
 * We return:
 *   a player data structure with the given key, name, an all-zero visibility
 *   string, and a random location on a room spot.
 * Caller is responsible for:
 *   later calling playerdelete
 */
static player_t* 
player_new(const char key, char* name, char* map)
{
  player_t* player = malloc(sizeof(player_t));
  if (player != NULL) {
    // 1) set key
    player->key = key;
    // 2) generate location (use random seed that varies with name)
    int seed = time(0);
    for (int i = 0; i < strlen(name); i++) {
      seed = seed * (int) name[i];
    }
    player->loci = grid_randloc(map, seed);
    // 3) allocate space to and setname
    player->name = malloc(strlen(name) + 1);
    if (player->name != NULL) {
      strcpy(player->name, name);
    // 4) allocate and set visibility
      int visibSize = strlen(map);
      player->visib = malloc(visibSize + 1);
      if (player->visib != NULL) {
        for (int i = 0; i < visibSize; i++) {
          player->visib[i] = '0'; // initializes everything as 'never seen'
        }
    // 5) Return
        return player;
      }
    }
  }
  // errors run off here
  fprintf(stderr, "Error: memory allocation errors in player_new\n");
  return NULL;
}

/*** players_get ***/
/* Get a player from a player list by its character key.
 *
 * Caller provides:
 *   valid pointer to player set
 * We return:
 *   the player indexed by the given character key if it is in the set,
 *   null otherwise
 */
static player_t*
players_get(players_t* players, const char key)
{
  if (players != NULL && players->list != NULL) {
    int listKey = (int) key;
    player_t* player = nuggset_get(players->list, listKey);
    return player;
  }
  return NULL;
}

/*************** players_getLoci ***************/
/* see players.h for description */
int 
players_getLoci(players_t* players, const char key)
{
  player_t* player = players_get(players, key);
  if (player != NULL) {
    return player->loci;
  }
  return -1;
}

/**************** players_move() ****************/
/* see players.h for description */
void 
players_move(players_t* players, const char key, const int newLoci)
{
  player_t* player = players_get(players, key);
  if (player != NULL) {
    player->loci = newLoci;
  }
}

/**************** players_getVisibility() ****************/
/* see players.h for description */
char* 
players_getVisibility(players_t* players, const char key)
{
  player_t* player = players_get(players, key);
  if (player != NULL) {
    return player->visib;
  }
  return NULL;
}

/**************** players_setVisibility() ****************/
/* see players.h for description */
void 
players_setVisibility(players_t* players, const char key, char* visibility)
{
  if (visibility != NULL) {
    player_t* player = players_get(players, key);
    if (player != NULL && strlen(player->visib) == strlen(visibility)) {
      strcpy(player->visib, visibility);
    }
  }
}

/*************** players_getName ***************/
/* see players.h for description */
char* 
players_getName(players_t* players, const char key)
{
  player_t* player = players_get(players, key);
  if (player != NULL) {
    return player->name;
  }
  return NULL;
}

/**************** players_addToMap() ****************/
/* see players.h for description */
void 
players_addToMap(players_t* players, char* map, const char self)
{
  if (players != NULL && players->list != NULL && map != NULL) {
    // fill in map with character keys
    nuggset_iterate(players->list, map, addToMap);

    // modify own marker from character key to @
    player_t* player = players_get(players, self);
    if (player != NULL && player->loci >= 0 && player->loci < strlen(map)) {
      map[player->loci] = '@';
    }
  }
}

/*** addToMap() ***/
/* Helper Function to players_addToMap: sets a player's location on a map string
 * to their character key.
 *
 * no return value.
 */
static void
addToMap(void* arg, const int listKey, void* playerPointer)
{
  // properly cast arguments
  char* map = arg;
  char key = (char) listKey;
  player_t* player = playerPointer;
  // add to map
  if (map != NULL && player->loci < strlen(map) && player->loci >= 0) {
    map[player->loci] = key;
  }
}

/**************** players_remove() ****************/
/* see players.h for description */
void 
players_remove(players_t* players, const char key)
{
  players_move(players, key, -999);
}

/**************** players_delete() ****************/
/* see players.h for description */
void 
players_delete(players_t* players)
{
  if (players != NULL) {
    if (players->list != NULL) {
      nuggset_delete(players->list, playerdelete);
    }
    free(players);
  }
}

/*** playerdelete() ***/
/* Helper function to players_delete. Deletes a player by freeing its name and 
 * visibility strings, then freeing the player itself.
 *
 * no return value.
 */
static void 
playerdelete(void* playerPointer)
{
  player_t* player = playerPointer;
  if (player != NULL) {
    // carefully free visibility
    if (player->visib != NULL) {
      free(player->visib);
    }
    // carefully free name
    if (player->name != NULL) {
      free(player->name);
    }

    free(player);
  }
}


/* ****************************************************************** */
/* ************************* UNIT_TEST ****************************** */
/*
 * This unit test creates a player set, adds players to it, uses it to modify a 
 * sample map, and deletes it in order to test the functionality of players 
 * methods and check for memory leaks.
 *
 * The test is run with command line
 *   ./playerstest
 * and should have no leaks if run with valgrind.
 */

#ifdef UNIT_TEST

int
main()
{
  grid_t* grid = grid_new("../maps/visdemo.txt");
  char* map = grid_getStr(grid);
  players_t* ps = players_new();

  char key1 = players_add(ps, "Nick", map);
  char key2 = players_add(ps, "Norma", map);
  players_add(ps, "Nate", map);
  char key4 = players_add(ps, "Nancy", map);

  printf("%c as key:\n", key2);
  players_addToMap(ps, map, key2);
  puts(map);

  printf("%c as key:\n", key4);
  players_addToMap(ps, map, key4);
  puts(map);

  printf("Should be equal: %s & %s\n", "Nick", players_getName(ps, key1));
  for (char key = 'A'; key <= players_getLast(ps); key++) {
    printf("%s is at %d, labeled %c.\n", players_getName(ps, key), 
                                         players_getLoci(ps, key), key);
  }

  players_delete(ps);
  grid_delete(grid);
}

#endif // UNIT_TEST
