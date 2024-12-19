/*
 * players.h - header file for players module
 *
 * The *players* data structure is a nugget set wrapper. It implements pairs
 * of (char,player) items using the local 'player' data structure.
 * It adds functions to:
 *   - create an instance of the player data type
 *   - get and set a player's location and visibility string
 *   - get a player's name
 *   - draw all stored players onto a map
 *
 * Rachel Matthew, February 2023
 */

#ifndef __PLAYERS_H
#define __PLAYERS_H

/*************** global types ***************/
typedef struct players players_t;

/**************** functions ****************/

/**************** players_new ****************/
/* Create an empty list of players.
 *
 * We return:
 *   pointer to the new players data structure, or NULL if error.
 * We guarantee:
 *   The player list is initialized empty.
 * Caller is responsible for:
 *   later calling players_delete.
 */
players_t* players_new(void);

/*************** players_getLast ***************/
/* Get the character key of the last added player.
 *
 * Caller provides:
 *   valid pointer to a players set
 * We return:
 *   the character key of the last added player if any players have been added
 *   '@' otherwise
 */
char players_getLast(players_t* players);

/*************** players_add ***************/
/* Add a player to a players list and return their character key.
 *
 * Caller provides: 
 *   valid pointer to a players set, a non-null name, and a non-null map.
 * We return:
 *   the player's character key if the player was successfully added;
 *   '[' otherwise.
 * We guarantee:
 *   The added player has a character key unique from all previous players.
 *   The added player has the given name.
 *   The added player has a visibility string of all zeroes.
 *   The added player's location is a random room spot not filled by a gold-pile
 *   or other player.
 */
char players_add(players_t* players, char* name, char* map);

/*************** players_getLoci ***************/
/* Get a player's location integer.
 *
 * We return:
 *   -1 if the player character key is not in players
 *   the player's integer location otherwise
 */
int players_getLoci(players_t* players, const char key);

/*************** players_move ***************/
/* Change a player's location integer.
 *
 * Caller provides:
 *   valid pointer to players
 * We guarantee:
 *   if a player with the given character key is in players, their location is
 *   set to the given newLoci
 *
 * no return value.
 */
void players_move(players_t* players, const char key, const int newLoci);

/*************** players_getName ***************/
/* Get a player's name string.
 *
 * We return:
 *   null if the player character key is not in players
 *   the player's name string otherwise
 * Caller is responsible for:
 *   NOT freeing the returned string
 */
char* players_getName(players_t* players, const char key);

/*************** players_getVisibility ***************/
/* Get a player's visibility string. 
 *
 * We return:
 *   null if the player character key is not in players
 *   the player's visibility string otherwise
 * Caller is resposible for:
 *   NOT freeing the returned string
 */
char* players_getVisibility(players_t* players, const char key);

/*************** players_setVisibility ***************/
/* Set a player's visibility string. 
 *
 * We guarantee:
 *   if visibility is null, nothing happens, otherwise;
 *   The player's visibility string will be set to visibility and the player's
 *   previous visibility string will be freed.
 * Caller is responsible for:
 *   later calling a function to delete the player in order to free the string
 * 
 * no return value.
 */
void players_setVisibility(players_t* players, const char key, char* visibility);

/*************** players_addToMap ***************/
/* Add players to a map string.
 *
 * We guarantee:
 *   all players in the player set are added to the map as alphabetic characters
 *   except for self, which is added as '@'
 *
 * no return value.
 */
void players_addToMap(players_t* players, char* map, const char self);

/*************** players_remove ***************/
/* Remove a player from the set.
 *
 * We guarantee:
 *   if the player is in the set, their location is set to -999, so they will no
 *   longer be drawn on the map, but they will not be deleted yet.
 *
 * no return value.
 */
void players_remove(players_t* players, const char key);

/*************** players_delete ***************/
/* Delete a set of players.
 *
 * We guarantee:
 *   The player set, all players, and all player strings are freed.
 * Caller is responsible for:
 *   Not attempting to call the set again
 *
 * no return value.
 */
void players_delete(players_t* players);

#endif // __PLAYERS_H
