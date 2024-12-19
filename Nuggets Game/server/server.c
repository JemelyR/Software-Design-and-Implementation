/* 
 * server.c - CS50 nuggets group final project
 *
 *
 * Jemely Robles, February 2023, cs50, 23W
 * Modified by Rachel Matthew, March 2023
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <math.h>
#include "grid.h"
#include "gold.h"
#include "grid.h"
#include "message.h"
#include "players.h"

/*************** GLOBAL TYPES ***************/
typedef struct spectator{
  addr_t client;
}spectator_t;

typedef struct game{
  grid_t* grid;
  players_t* players;
  spectator_t* spectator;
  gold_t* gold;
  char* map;
  int seed;
}game_t;

/*************** GLOBAL VARIABLE ***************/
game_t* game;

/**************** FUNCTIONS ****************/
/* Local Function Prototypes */
/* General */
static void initialize(char* mapFilename, int seed);
static game_t* gameInit(char* mapFile, int seed);
static bool handleMessage(void* arg, const addr_t client, const char* message);
static void gameOver(const addr_t* client, players_t* players);
static char addPlayer(char* playerName);
static char getMapCharacter(int loci);

/* Spectator-related */
static spectator_t* spectatorAdd(addr_t client);
static void spectatorRemove(spectator_t* spectator);
static bool isSpectator(addr_t client);

/* Message-related */
static void okayMessage(const addr_t client, char playerChar);
static void gridMessage(const addr_t client);
static void goldPlayerMessage(const addr_t client, char playerChar, 
                              int newGold);
static void goldSpectatorMessage(const addr_t client);
static void goldMessageAll(addr_t* clients, char exception);
static void displayMessage(const addr_t client, char player);
static void displayMessageAll(addr_t* clients);

/**************** main() ****************/
int 
main(const int argc, char* argv[])
{
  // Initialize client array
  addr_t clients[26];
  for (int i = 0; i < 26; i++) {
    clients[i] = message_noAddr();
  }

  // Validate parameters
  //return 1 for unparseable arguments (see initialize) or 2 for other errors
  if (argc < 2 || argc > 3) {
    fprintf(stderr, "Error: %d is an invalid # of arguments to %s. Need 2-3.\n",            argc-1, argv[0]);
    return 2;
  } else if (argc == 2) { //if seed is absent, generate a random one
    initialize(argv[1], getpid());
  } else if (argc == 3) { //if seed is given
    const int seed = atoi(argv[2]);
    initialize(argv[1], seed);
  } 

  // Initialize the 'message' module
  int port = message_init(stderr);
  if (port == 0) {
    return 2;
  }
  // message loop
  bool listen = true;
  while (listen && game->grid != NULL) {
    listen = message_loop(&clients, 0.0, NULL, NULL, handleMessage);
  }

  //call gameOver() to inform all clients the game has ended in case of the 
  //message loop terminated due to error
  if (game->grid != NULL) {
    gameOver(clients, game->players);
  }

  // Clean up
  free(game);
  message_done();
  return 0;
}

/**************** initialize() ****************/
/* Validate command-line arguments and call gameInit. 
 * Exit 1 in case of invalid argument.
 */
static void
initialize(char* mapFilename, const int seed)
{
  // Validate map.txt
  FILE* mapfile = fopen(mapFilename, "r");
  if (mapfile == NULL){
    fprintf(stderr, "Error: map file cannot be read.\n");
    exit(1);
  } else {
    fclose(mapfile);
  }
  // Validate seed:
  if (seed <= 0){
    fprintf(stderr, "Error: seed must be a positive integer.\n");
    exit(1);
  }

  // Arguments Valid -> MAKE GAME
  game = gameInit(mapFilename, seed);

  // Validate game
  if (game == NULL) {
    fprintf(stderr, "; failed to generate game\n"); // ';' present b/c specifics
    exit(1);                                        // printed in game_init
  }
}

/**************** handle_message() ****************/
/* Handle messages received from clients to maintain the gameplay loop for all 
 * users. Can parse messages of the following formats (parentheses denote the 
 * piece is optional):
 *   PLAY
 *   SPECTATE
 *   QUIT( %s)
 *   KEY %c( %c)
 *
 * Returns true.
 */
static bool
handleMessage(void* arg, const addr_t client, const char* message)
{
  // get clients
  addr_t* clients = arg;

  //check message type and handle accordingly
  if (strncmp(message, "PLAY ", 5) == 0) {
    char playerName[30] = "";
    for (int i = 5; i < strlen(message); i++) {
      playerName[i-5] = message[i];
    }

	  // if there are already the maximum # of players:
    if(players_getLast(game->players) == 'Z'){
      message_send(client, "QUIT Game is full: no more players can join.");
    // otherwise:
    } else {
      char playerChar = addPlayer(playerName);
      if (playerChar == ('Z' + 1)) {
        message_send(client, "QUIT Cannot initialize new player.");

      } else {
        //respond with OK
        okayMessage(client, playerChar);

        //send the client GRID and GOLD messages
        gridMessage(client);
        goldPlayerMessage(client, playerChar, 0);

        //store the client in clients
        int locus = (int) (playerChar - 'A');
        clients[locus] = client;
      }
    }

  
  } else if (strncmp(message, "SPECTATE", 8) == 0) {
	  // if the server has a spectator, send them a QUIT message
    if (game->spectator != NULL) {
      message_send(game->spectator->client, 
                   "QUIT You have been replaced by a new spectator.");
      spectatorRemove(game->spectator);
    }
    game->spectator = spectatorAdd(client);

	  //send the client GRID and GOLD  messages
    gridMessage(client);
    goldSpectatorMessage(client);


  } else if (strncmp(message, "QUIT", 4) == 0) {
    // if client is player
    if (!isSpectator(client)) {
      char playerChar;
      sscanf(message, "QUIT %c", &playerChar);
      message_send(client, "QUIT Thanks for playing!");
      players_remove(game->players, playerChar); 
    
    // if client is spectator
    } else { // if client is spectator
      message_send(client, "QUIT Thanks for watching!");
      spectatorRemove(game->spectator);
    }
  
  
  } else if (strncmp(message, "KEY ", 4) == 0) {
    char key, playerChar;
    sscanf(message, "KEY %c %c", &key, &playerChar);

	  //if the key is Q, respond with a QUIT message and call players_remove()
    if (key == 'Q') {
      if (!isSpectator(client)) { //if client is a player
        message_send(client, "QUIT Thanks for playing!");
        players_remove(game->players, playerChar);
      } else { //else client is spectator
        message_send(client, "QUIT Thanks for watching!");
        spectatorRemove(game->spectator);
      }

	  //otherwise, move the player
    } else {
      int curloci = players_getLoci(game->players, playerChar);
      int x = grid_NtoX(game->grid, curloci);
      int y = grid_NtoY(game->grid, curloci);
      switch (key) {
      case 'h': x--; break;       //move left
      case 'l': x++; break;       //move right
      case 'j': y++; break;       //move down
      case 'k': y--; break;       //move up
      case 'y': y--; x--; break;  //move diagonal up and left
      case 'u': y--; x++; break;  //move diagonal up and right
      case 'b': y++; x--; break;  //move diagonal down and left
      case 'n': y++; x++; break;  //move diagonal down and right
      default: break;
      }
      int newloci = grid_XYtoN(game->grid, x, y);
      char dir = getMapCharacter(newloci);
	    //if the direction is wall, ignore it
      if (dir != ' ' && dir != '-' && dir != '|' && dir != '+' && dir != '\n') {
        players_move(game->players, playerChar, newloci);
	 	    //if the direction is player, call players_move() for other player
        char last = players_getLast(game->players);
        for (char otherPlayer = 'A'; otherPlayer <= last; otherPlayer++){
          if (dir == otherPlayer) {
            players_move(game->players, otherPlayer, curloci);
          }
        }

        //if the direction is gold, call gold_find() and send GOLD message(s)
        if (dir == '*') {
          int collected = gold_get(game->gold, newloci);
          gold_find(game->gold, playerChar, newloci);
          goldPlayerMessage(client, playerChar, collected);
          goldMessageAll(clients, playerChar);

          // check if game has ended
          if (gold_getRemaining(game->gold) == 0) {
            gameOver(clients, game->players);
            return true;
          }
        }
      }
    }
  }
  // send all clients DISPLAY messages 
  displayMessageAll(clients);

  return true;
}

/**************** gameOver() ****************/
/* Sends all clients GAMEOVER & QUIT messages and frees the game. */
static void
gameOver(const addr_t* clients, players_t* players)
{
  //initialize gameover string
  char str[27*49 + 10];
  strcpy(str, "GAMEOVER\n         Player Name           Character  Score\n");
  //for each player in the player list
  int playersAmount = (int) (players_getLast(players) - 'A');
  for (int i = 0; i <= playersAmount; i++){
    //add the player's name, corresponding character, and purse value to the 
    //gameover string
    char playerChar = 'A' + i;
    char* playerName = players_getName(players, playerChar);
    int purse = gold_getPurse(game->gold, playerChar);

    char result[49]; // assume 4-digit purse value & 30-char player name
    sprintf(result,"%-30.30s|        %c| %4d\n", playerName, playerChar, purse);
    strcat(str, result);
  }

  // To each client:
  for (int i = 0; i <= playersAmount; i++) {
    addr_t client = clients[i];
    if (message_isAddr(client)) {
      //send DISPLAY message to client containing the gameover string
      message_send(client, str);
      //send QUIT message to client informing them the game is over
      message_send(client, "QUIT Thank you for playing.");
    }
  }
  if (game->spectator != NULL) {
    message_send(game->spectator->client, str);
    message_send(game->spectator->client, "QUIT Thank you for watching.");
  }

  // Now that messages have been sent, clean up game
  grid_delete(game->grid);
  game->grid = NULL;
  players_delete(game->players);
  gold_delete(game->gold);
  spectatorRemove(game->spectator);
}

/**************** gameInit() ****************/
/* Initializes the game modules.
 * Returns the game if successful, NULL otherwise.
 */
static game_t*
gameInit(char* mapFile, int seed)
{
  game_t* game = malloc(sizeof(game_t));
  if (game == NULL) {
    fprintf(stderr, "Error: failed to allocate space to game");
    return NULL;
  }
  
  // Set seed
  game->seed = seed;
  // Generate grid from map file and save map
  game->grid = grid_new(mapFile);
  if (game->grid == NULL) {
    fprintf(stderr, "Error: failed to generate grid");
    return NULL;
  }
  game->map = grid_getStr(game->grid);

  // Generate gold-piles from map and seed
  const int totalGold = 345;
  const int maxPile = 28;
  const int minPile = 4;
  game->gold = gold_new(totalGold, minPile, maxPile, game->map, seed);
  if (game->gold == NULL) {
    fprintf(stderr, "Error: failed to generate gold-piles");
    return NULL;
  }

  // Set empty players set and spectator slot
  game->players = players_new();
  if (game->players == NULL) {
    fprintf(stderr, "Error: failed to generate players set");
    return NULL;
  }
  game->spectator = NULL;
  
  return game;
}

/**************** spectatorAdd() ****************/
/* Saves the given client as the game's spectator. */
static spectator_t*
spectatorAdd(addr_t client)
{
  spectator_t* spectator = malloc(sizeof(spectator_t));
  if (spectator == NULL){
    message_send(client, "QUIT Cannot add spectator.");
    fprintf(stderr, "Error: memory failured in initializing spectator.\n");
    return NULL;
  }
  spectator->client = client;
  game->spectator = spectator;
  return spectator;
}

/**************** spectatorRemove() ****************/
/* Frees the game's current spectator if necessary. */
static void
spectatorRemove(spectator_t* spectator)
{
  if (spectator != NULL) {
    free(spectator);
    game->spectator = NULL;
  }
}

/**************** isSpectator() ****************/
/* Return true if the given client is the stored spectator, false otherwise. */
static bool
isSpectator(addr_t client)
{
  if (game == NULL || game->spectator == NULL) {
    return false;
  }
  return message_eqAddr(client, game->spectator->client);
}

/* filledMap() */
/* Helper function to helper functions: allocates and fills a version of the
 * game map containing all players and all gold-piles.
 *
 * Used in the following functions (defined below):
 *   addPlayer
 *   getMapCharacter
 *
 * The resulting string MUST BE freed by the caller later.
 */
static char*
filledMap() 
{
 // create duplicate map to avoid changing the original
  char* map = malloc(strlen(game->map) + 1);
  strcpy(map, game->map);
  // fill duplicate map with players and gold
  players_addToMap(game->players, map, '~');
  gold_addToMap(game->gold, map);

  return map;
}

/*** addPlayer() ***/
/* Add a player to the game, taking into account the position of not only walls
 * and passages but also gold-piles and other players.
 *
 * Returns the character key of the newly-added player.
 */
static char 
addPlayer(char* playerName)
{
  char* map = filledMap();
  char playerChar = players_add(game->players, playerName, map);
  free(map);
  return playerChar;
}

/*** getMapCharacter() ***/
/* Get and return the character at a given location on the game map, taking into
 * account gold-piles and players.
 */
static char
getMapCharacter(int loci)
{
  char* map = filledMap();
  char c;
  if (0 <= loci && loci < strlen(map)) {
    c = map[loci];
  } else {
    c = '~';
  }
  free(map);
  return c;
}

/******************** Messaging Helper Functions ********************/
/*** okayMessage() ***/
static void
okayMessage(const addr_t client, char playerChar)
{
  char message[5];
  sprintf(message, "OK %c", playerChar);
  message_send(client, message);
}

/*** gridMessage() ***/
static void
gridMessage(const addr_t client)
{
  char message[28]; // assumes rows and columns will be five digit #s or less
  
  int rows = grid_getY(game->grid);
  int cols = grid_getX(game->grid);
  sprintf(message, "GRID %d rows, %d cols", rows, cols);

  message_send(client, message); 
}

/*** goldPlayerMessage() ***/
static void 
goldPlayerMessage(const addr_t client, char playerChar, int newGold)
{
  char message[77]; // assumes all gold values have maximum of 4 digits

  int remaining = gold_getRemaining(game->gold);
  int purse = gold_getPurse(game->gold, playerChar);
  if (newGold != 0) {
    sprintf(message, 
      "GOLD Player %c has %d nuggets (%d nuggets unclaimed). GOLD received: %d",
      playerChar, purse, remaining, newGold); 
  } else {
    sprintf(message, "GOLD Player %c has %d nuggets (%d nuggets unclaimed).", 
            playerChar, purse, remaining);
  }
  
  message_send(client, message);
}

/*** goldSpectatorMessage() ***/
static void
goldSpectatorMessage(const addr_t client)
{
  char message[29]; // assumes nuggets unclaimed # has a maximum of 4 digits
  
  int remaining = gold_getRemaining(game->gold);
  sprintf(message, "GOLD %d nuggets unclaimed.", remaining); 
  
  message_send(client, message);
}

/*** goldMessageAll() ***/
/* Sends an up-to-date gold message to all clients */
static void
goldMessageAll(addr_t* clients, char exception)
{ 
  for (char key = 'A'; key <= 'Z'; key++) {
    if (key != exception) {
      int index = (int) (key - 'A');
      addr_t client = clients[index];
      if (message_isAddr(client) && players_getLoci(game->players,key) != -999){
        goldPlayerMessage(client, key, 0);
      }
    }
  }

  if (game->spectator != NULL) {
    goldSpectatorMessage(game->spectator->client);
  }
}

/*** displayMessage() ***/
static void
displayMessage(const addr_t client, char player) 
{
  // get visibility string (null for spectator)
  char* vis;
  if (player == ' ') {
    vis = NULL;
  } else {
    vis = players_getVisibility(game->players, player);
    // and ensure visibility is up-to-date for player location
    int x = grid_NtoX(game->grid, players_getLoci(game->players, player));
    int y = grid_NtoY(game->grid, players_getLoci(game->players, player));
    visibility(game->grid, vis, x, y);
  }

  // generate display with visibility string
  char* display = grid_display(game->grid, game->players, game->gold, 
                               player, vis);
  
  // send message to client
  if (display != NULL) {
    char message[9 + strlen(display)];
    sprintf(message, "DISPLAY\n%s", display);

    message_send(client, message); 
    free(display);
  }
}

/*** displayMessageAll() ***/
/* Sends an up-to-date display message to all clients */
static void
displayMessageAll(addr_t* clients)
{
  for (char key = 'A'; key <= 'Z'; key++) {
    int index = (int) (key - 'A');
    addr_t client = clients[index];
    if (message_isAddr(client) && players_getLoci(game->players, key) != -999) {
      displayMessage(client, key);
    }
  }

  if (game->spectator != NULL) {
    displayMessage(game->spectator->client, ' ');
  }
}
