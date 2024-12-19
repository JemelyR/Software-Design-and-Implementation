# CS50 Nuggets
## DR.JR, Winter 2023

### server

The **server** starts from the command line, where the first argument is the pathname for a map file and the second argument is an optional seed for the random-number generator. The server accepts up to 1 spectator and up to 26 players.
### Usage

The server uses functions defined in gold.h, grid.h, players.h and message.h. It verifies its arguments, sending error messages when an error occurs. If the optional seed is provided, the server passes it to srand(seed), otherwise the server uses srand(getpid()). Server verifies the map file can be opened for reading, assuming map is valid. Server initializes the game and network, waiting for messages from clients, who are either players or spectators, reacting to each type of message. Server exports the following functions:

```c
int main(const int argc, char* argv[]);
void initialize(char* map, const int seed);
static bool handle_message(void* arg, const addr_t client, const char* message);
void gameOver(const addr_t* clients, players_t* players);
game_t* game_init(char* mapFile, int seed);
spectator_t* spectator_add(addr_t client);
void remove_spectator(spectator_t* spectator);
bool is_spectator(game_t* game, addr_t client);
```

### Implementation

A spectator structure using `addr_t` is made to keep track of spectators, and a game structure is also made. The game structure holds pointers to the grid, players, spectator, gold, map, and seed.

The `main` function starts up client, parses the command line and validates parameters, before calling for game and message initialization 

The `initializer` function checks the validity of given arguments and makes the game.

`handle_message` gets a hold of client and depending on message type, handle their message accordingly

The `gameOver` function initializes a gameover string, and clears up the game.

`game_init` initializes the game structure.

`spectator_add` takes a client and turns them into the game’s spectator.

`remove_spectator` removes a given spectator from the game.

` is_spectator` checks if the client is a spectator for the game.


### Assumptions

No assumptions beyond those that are clear from the spec.

### Files

* `Makefile` - a compilation procedure
* `server.c` - the implementation
* `gold.c` - module for gold
* `gold.h` - interface of gold module
* `grid.c` - module for grid
* `grid.h` - interface of grid module
* `players.c` - module for players
* `players.h` - interface of players module

### Compilation

To compile use "./server . . .", with 2 or 3 arguments

### Testing
Tested using server to client connections



