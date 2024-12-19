# CS50 Nuggets
## Implementation Spec
### DR.JR, Winter, 2023

According to the [Requirements Spec](REQUIREMENTS.md), the Nuggets game requires two standalone programs: a client and a server.
Our design also includes gold, grid, and player modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Plan for division of labor

Server : Jemely Robles.

Client: Daniel Ha.

Players and Gold Modules: Rachel Matthew.

Grid module: Rose Wenxin Zhao.

Testing and debugging for all modules done by all team members interchangeably.


## Client

### Data Structures

### main
```c
int main(const int argc, char* argv[]);
```
Executes program, calling initializer and client-handler modules
Pseudocode:
	
	  initialize the client
	  determine if client is a player or spectator
	  if the arguments are invalid, exit non-zero
	  initialize address of server from arguments
	  if the role is player
		  initialize string message PLAY
	  get full username, taking spaces into consideration
		  if argument is too long
			  print error message
			  exit non-zero
		  inform server of player
		  start message loop, if the answer is OK, then the game loop begins
		  return 0
	  else the role is spectator
		  send message SPECTATE to server
		  handle client
	  finish message

### initializer
```c
char initializer(int argc, char* argv[]);
```
Takes user input from main() and initializes the client as a player or as spectator.
Pseudocode:

	  if there are less than 3 argument
		  print error message
		  return f
	  make sure port and host are valid
	  if not valid
		  print error message
		  finish message
		  return f
	  if message initialization is null
		  print error message
		  return f
	  if there are more than 4 arguments
		  return p
	  else return s

### clientHandler
```c
static void clientHandler(void* argc, const addr_t server);
```
Uses ncurses to monitor and manage both user and server inputs.
Pseudocode:

	  cast and save identity character
	  if argument not null
		  the identity is set to the identity pointer
	  initialize screen
	  get the dimensions of the screen
	  get the dimensions of the grid
	  start message loop
	  if the dimensions of the map are larger than the actual window
		  print error message
		  exit non-zero
	  start the gameplay loop
	  while running 
		  if input is received, send message
		  manage player input
		  if arguments not null and input is valid
			  send KEY and key input to server
		  else if argument is null and input is Q
			  send QUIT to server
		  update display
		  check for QUIT message from server
		  if you do get QUIT
			  exit the game run
		  refresh 
	  end screen if not already ended
	  print quit message

---
## Server

### Data structures

A spectator structure using `addr_t` is made to keep track of spectators, and a game structure is also made. The game structure holds pointers to the grid, players, spectator, gold, map, and seed.

### Definition of function prototypes

### main
```c
int main(const int argc, char* argv[]);
```
Starts up client, parses the command line and validates parameters, before calling for game and message initialization 
Pseudocode:

	  initialize clients and client size, using message_noaddr func from message module
	  if two arguments are given
		  initialize the game using given map, and use getpid as seed
	  if three arguments are given
		  initialize game using map and given seed
	  else print error and return non zero
	  initialize the message module and print the port number on which we wait
	  initialize a bool listen
	  while listen and game to grid pointer are not null
		  message loop with clients using handle message
	  if game to grid pointer is not null
		  call gameover to tell clients the game has ended
	  clean up by freeing the game and calling to end message
	  return 0

### initialize
```c
void initialize(char* map, const int seed);
```
Checks the validity of given arguments and make the game.
Pseudocode:

	  validate map.txt
	  if the mapfile is null 
      print an error message and exit non-zero
	  else close the map file
	  if the seed is less than 0
		  print an error message and exit non-zero
	  initialize the game using the map and seed
	  if the game is null
		  print an error message and exit non-zero

### handle_message
```c
static bool handle_message(void* arg, const addr_t client, const char* message);
```
Gets a hold of the client and depending on the message type, handle their message accordingly.
Pseudocode:

    get the clients
    check the message and handle accordingly
	  if the message is PLAY
		  grab the player's name
		  if there are already max amount of players
			  send message to client telling them game is full
		  else add a player using client's name
		  send error message if player not initialized
		  else respond with an ok message
			  send grid and gold message to client
			  store the client in clients
	  if message is SPECTATE
		  if there is already a spectator
			  send a message to the current spectator that they're being replaced
        remove current spectator
    set game to spectator pointer to client by adding client to spectator
    send client grid and gold messages
    if the message is KEY
      scan the message for the key input
      if the key is Q
        if the client is a player
          send them a thanks for playing message
          remove that player	
        else client is a spectator 
          send them a thanks for watching message
          remove the spectator
      else the key is to move the player
        get the current location of the player
        if the key is h, move the player left
        if the key is l, move them right
        if the key is j, move them down
        if the key is k, move up
        if the key is y, move diagonal up and left
        if the key is u, move diagonal up and right
        if the key is b, move diagonal down and left
        if the key is n, move diagonal down and right
        get new location
        get char in this new location
        if the new location is not a wall
          update player location
        if the new location is gold
          give player that gold
          update player's gold message
        update how much gold is left
        if there is no gold left
          call game over
      return true	
	  display messages for everyone
	  return true

### gameOver
```c
void gameOver(const addr_t* clients, players_t* players);
```
Initializes a gameover string, and clears up game.
Pseudocode:

	  allocate space in a string for gameover message
	  initialize string content, with player name character and score
	  for each player
		  get the player char
		  get the player name based on that char
		  get that players gold amount
		  initialize a result string
		  store the results in order
		  append the result string to the game over string
	  for each client
		  get the client
		  send gameover message to client
		  send quit message to client
	  if the game has a spectator
		  send the spectator the gameover string
		  send the spectator a quit message
	  delete the grid
	  set game to grid pointer to null
	  delete players
	  delete the gold
	  remove spectator

### game_init
```c
game_t* game_init(char* mapFile, int seed);
```
Initialize game structure.
Pseudocode:

	  initialize game structure, with size of game_t
	  if the game is null
		  print error message
		  return null
	  set game to seed pointer as seed
	  set game to grid pointer as new grid
	  if the game to grid pointer is null
		  print an error message
		  return null
	  set game to map pointer to grid string
	  initialize total gold, max pile and min pile
	  set game to gold pointer to new gold
	  if the result is null
		  print error message
		  return null
	  set game to players pointer as new players
	  if the result is null
		  print error message
		  return null
	  set game to spectator pointer as null
	  return the game
	
### spectator_add
```c
spectator_t* spectator_add(addr_t client);
```
Takes a client and turns them into the game's spectator.
Pseudocode:

	  initialize a spectator struct with size of spectator_t
	  if the spectator is null
		  send error message to client
		  print error message
		  return  null
	  set spectator to client pointer to client
	  set game to spectator pointer to spectator
	  return the spectator

### remove_spectator
```c
void remove_spectator(spectator_t* spectator);
```
Removes a given spectator.
Pseudocode:

	  if the spectator is not null
		  free the spectator
		  set game to spectator pointer to null

### is_spectator
```c
bool is_spectator(game_t* game, addr_t client);
```
Checks if the client is a spectator for the game.
Pseudocode:

	  if the game or game spectator are null
		  return false
	  else initialize spectator as game to spectator pointer
	  check if client and spectator to client have same address
	  return result

---
## Player

Players are handled in the players.c module, whose functions are described in player.h.

### Data structures
We use two data structures: a player structure,` player_t`, holding the players char, their name string, visibility string and location in the map string, called by a players structure, holding all of the players in a nuggset list. The nuggset structure maintains an unordered collection of (key, item) pairs;
Both start empty.

### Definition of function prototypes

### players_new
```c
players_t* players_new(void);
```
Creates an empty list of players, returning pointer to new players data structure, initialized empty.
Pseudocode:

	  initialize a player struct with a memory allocation of the size of players_t
	  if the players is not null
		  set the pointer from players to list as a new nuggset
	  if the new nuggset is not null
		  set the nuggset head using the players to list pointer 
		  return players
	  send error message if players was null
	  return null

### players_getLast
```c
char players_getLast(players_t* players);
```
Add a player to the players list, returning their character key.
The added player has a character key, unique from previous players, and assigned a visibility string and located on a random spot in a room on the map not filled by gold.
Pseudocode:

	  if players and players pointer to list is not null
		  take the string of the nuggset head using the player to list pointer
		  return the string's last index;
	  else return the character before A

### players_add
```c
char players_getLast(players_t* players, char* name, char* map);
```
Add a player to a players list, returning their character key. 
Pseudocode:
	
	  if the player, player to list pointer, player name and map are not null
		  get player's key from meta information using update head func
		  if that key is bigger than Z
			  return the character after Z
		  else initialize a new player struct
		  if this player is not null
			  set the nuggset struct for this player as true
			  return the key
		  else print error
		  return the character after z

### players_getLoci
```c
int players_getLoci(players_t* players, const char key);
```
Get a player's location integer.
Pseudocode:

	  initialize a new player struct
	  if the player is not null
		  return the player to location pointer
	  return non-zero

### players_move
```c
void players_move(players_t* players, const char key, const int newLoci);
```
Change a players location integer.
Pseudocode:

	  initialize a new player struct
	  if the player is not null
		  set the player to location pointer to the new location;

### players_getName
```c
char* players_getName(players_t* players, const char key);
```
Return a player's name string
Pseudocode:

	  initialize a new player struct
	  if the player is not null
		  return the player to name pointer
	  else return null

### players_getVisibility
```c
char* players_getVisibility(players_t* players, const char key);
```
Get a player's visibility string 
Pseudocode:

	  initialize a player struct
	  if player is not null
		  return the player to visibility pointer
	  else return null

### players_setVisibility
```c
void players_setVisibility(players_t* players, const char key, char* visibility);
```
Set a player's visibility string
Pseudocode:

	  if visibility is not null
		  get the player being updated
		  if that player is not null and the pointer to visibility is equal to the given visibility
		  copy the given visibility to the player to visibility pointer

### players_addToMap
```c
void players_addToMap(players_t* players, char* mao, const char self);
```
Add the players to a map string for the game initialization
Pseudocode:

	  if the players are not null and the player to list pointer is not null and there is a map
		  iterate through nuggset to fill in map with the character keys
		  get a player 
		  if that player is not null, and has a location on the map
		  turn that players spot on the map to @ to represent themselves

### players_remove
```c
void players_remove(players_t* players, const char key);
```
Remove a player from the set, stop drawing them on the map but maintain their information for scoring.
Pseudocode:
	
	  move the player location to a spot outside the map using players move func

### players_delete
```c
void players_delete(players_t* players);
```
Fully delete a set of players, freeing all players, player sets, and strings.
Pseudocode:

	  set a player struct to player pointer
	  if the player is not null
		  if the players visibility is not null
			  free the players visibility
        if the players name is not null
        free the players name
      free the player 

---
## Gold
The gold module is handled in gold.c, whose functions are described in gold.h.

### Data structures
The gold module uses a gold structure, containing a list of gold-piles, map string length, and remaining gold.

### Definition of function prototypes

### gold_new
```c
gold_t* gold_new(const int total, const int minPiles, const int maxPiles, char* map, const int seed);
```
Creates a new set of gold-piles and store the given map string size.
Pseudocode:

	  check the validity of the parameter inputs
	  if they are not valid, print error message
	  else , initialize gold struct with size of gold_t
	  if the gold is not null
		  set gold to map size pointer to the string length of map
	    set the gold to remaining gold pointer to the total gold
		  set the gold to list pointer to a new nuggset
		  if the gold to list pointer is not null
			  use strand of seed
			  use random number of piles between the set min and max
			  assign randomly distributed values to each gold-pile
			  add piles to nuggset at random location
			  add to set
		  return gold
		  else free gold
	  else print error message
	  return null

### gold_get
```c
int gold_get(gold_t* gold, const int loci);
```
Get the value of a gold pile at a given location.
Pseudocode:

	  initialize a value to 0
	  if the gold and gold to list pointer are not null, and the location is less than mapsize
		  set a value pointer to the nuggset get of the list and location
		  if the value pointer is not null
			  set the value equal to the value pointer
	  return the value

### gold_getPurse
```c
int gold_getPurse(gold_t* gold, const char player);
```
Get the value in the purse of a given player.
Pseudocode:

	  initialize a value to 0
	  if the gold and gold to list pointer are not null
		  get the purse location 
		  set a value pointer to nuggset get using list and purse location
		  if the value pointer is not null
			  set value equal to value pointer
	  return value

### gold_getRemaining
```c
int gold_getRemaining(gold_t* gold);
```
Get the amount of gold yet to be found.
Pseudocode:

	  if gold is not null
		  return the gold to remaining gold pointer
	  else 
		  return non-zero

### gold_find
```c
bool gold_find(gold_t* gold, const char player, const int loci);
```
Transfer the gold at a given location to a given player’s purse.
Pseudocode:

	  if gold and gold to list pointer are not null
		  store gold from the given location and gold in players purse into a new value
		  create a new value pointer with the memory size of an integer
		  set this new value pointer to the new value
		  get the purse location
		  get the old value of gold in purse
		  if the old value is not null
			  free the old value
		  remove gold pile from remaining
		  remove fold pile from list and check that current values are correct
	  else return false

### gold_addToMap
```c
void gold_addToMap(gold_t* gold, char* map);
```
Add gold onto a map string.
Pseudocode:

	  if gold, gold to list pointer and map are not null
		  iterate through the nuggset add to map using addtomap func
	  add to map :
	  casts argument to char*
	  if map is not null and location is less that map string length
		  the map location is gold

### gold_delete
```c
void gold_delete(gold_t* gold);
```
Deletes the gold data structure.
Pseudocode:

	  if gold is not null
		  if gold to list pointer is not null
			  delete gold from the nugget set
			  if item is not null
			  free item
		  free the gold

---
## Grid
The grid module is handled in grid.c, whose functions are described in grid.h.

### Data structures
The grid module uses a grid structure, containing a string representation of the grid, and x and y coordinates

### Definition of function prototypes

### grid_new
```c
grid_t* grid_new(char* mapFile);
```
Reads the map file and creates a grid, returning a pointer to a new grid.
Pseudocode:

	  initialize a grid structure
	  if the grid is null
		  return null
	  else open map file as read
		  if the file is null
			  return null
	  initialize line string and grid string
	  initialize y coordinate to 0
	  while there is a line to get
		  copy that line to the end of the grid string
		  increase y by 1
	  initialize x coordinate to length of line string
	  set grid to x pointer to x
	  set grid to y pointer to y
	  set grid to grid string to grid string
	  close file
	  return grid

### grid_get
```c
char grid_new(char* map, int n);
```
Returns the char at the given location, without removing it from the map.
Pseudocode:

	  if map is null or n is greater than map length or n is less than 0
		  return \0
	  else return a char of what the location on the map has

### grid_randloc
```c
int grid_randloc(char* map, int seed);
```
Gets a random location index that is '.' on the grid.
Pseudocode:
	
	if map is null
		return non-zero
	use srand seed
	initialize a low value of 0 and a high value of the length of the map minus one
	get a random number between that low and high
	while the random location is not '.'
		keep getting a random number
	return the int of the location

### grid_NtoX
```c
int grid_NtoX(grid_t* grid, int n);
```
Converts linear coordinates of grid to x x-y coordinate, returning -1 if null.
Pseudocode:

	  if grid or grid to grid string pointer is null or n is greater than grid size or less than 0
		  return -1
	  else return the integer of the x coordinate.

### grid_NtoY
```c
int grid_NtoY(grid_t* grid, int n);
```
Converts linear coordinate of grid to y in x-y coordinate, return -1 if null.
Pseudocode:

	  if grid or grid to grid string pointer is null or n is greater than grid size or less than 0
		  return -1
	  else return the integer of the y coordinate.

### grid_XYtoN
```c
int grid_XYtoN(grid_t* grid, int x, int y);
```
Converts x-y coordinates of grid to linear coordinate/
Pseudocode:

	  if grid or grid to grid string pointer is null 
      if x is greater than grid x or less than 0, or y is greater than grid y or less than 0
		    return -1
	    else return the integer of the n coordinate.

### grid_display
```c
char* grid_display(grid_t* grid, players_t* players, gold_t* gold, char player, char* vis);
```
Displays the grid to clients, taking the players, gold, map, and visibility into account.
Pseudocode:

	  if the grid or grid string is null
		  return null
	  initialize map string as grid to grid string pointer
	  initialize output string
	  copy the map onto the output
	  add players to the map
	  add gold to the map
	  if the visibility is null
		  return the output string
	  for every index of the map
		  store the index of output string
		  store this index of visibility string
		  if the stores index of visibility is 0
			  this index of output is just a space
		  else if the index of visibility is 2 and index of output is a gold pile or another player
			  this index of output is equal to this index of map string
    return output

### grid_iterate
```c
void grid_iterate(grid_t* grid, void* arg, void (*item func)(void* arg, char* c));
```
Iterates over the grid, calling a function on each character.
Pseudocode:

    if grid or grid string or item func are not null
		  initialize grid string
      initialize a char c
      initialize an integer i as 0	
      while the char c of the grid string is not the end of file
	      call item func on the arg and grid string + i
	      increase i

### grid_delete
```c
void grid_delete(grid_t* grid);
```
Deletes the grid, freeing it and the grid string.
Pseudocode:

	  if grid is not null
		  free grid to grid string pointer
		  free grid

### visibility
```c
char* visibility(grid_t* grid, char*vis, int px, int py);
```
Updates visibility string based on player location.
Pseudocode:

    initialize grid string
    initialize the n coordinate of the player location
    store the char c of the character at the n index of grid string
    if c is not '.' and not '#'
      print error message	
      return vis
    for each point in the grid string
      store the point at this index
      if this point is not a space
        if the point is not '\n'
          turn visibility at this index to 1
        else
          use isVisible function to check if the point is visible by the player
		  if isVisible returns 0
			if the point corresponds to a 1 or 2 on the vis string
				change its value on vis to 2
			else
				change to 0
		  else 
			change vis at this index to 1
    return vis	

### isVisible
```c
static int isVisible(grid_t* grid, int n, int px, int py);
```
Check if the point is visible by the player.
Pseudocode:

    Check where the point is relative to the player. 
	  6 cases:
		  1. point and player are on the same row or +/- 1 row
		  2. point and player are on the same column or +/- 1 column
		  3. point is at left above of player
		  4. point is at right below of player
		  5. point is at left below of player
		  6. point is at right above of player
	  for the first 2 cases, check the row/column of points in between them
		  if any point is not '.', return 0
	  for the other 4 cases, check every point on the line that connect them 
		  to find these points, calculate tangent of the line, and get the corresponding y_intercepts for all x's in between them, and x_intercepts for all y's in between them
		  for each point,
			  if those points will block the visibility, i.e. '-', '|', '+', "#', etc. (which will depend on its location and cases), return 0
	  if reaching to the end of checks, return 1


## Testing plan

### unit testing

The units Gold, Grid and Players are tested using smaller unit tests.

The gold testing creates, modifies, prints, and deletes a gold set to test the functionality of gold methods and check for memory leaks. The test is run with command line `./goldtest`.

The grid testing has four test cases, using smaller grid strings to test functionality of grid methods and check for memory leaks.

The players testing creates a player set, adds players to it, uses it to modify a sample map, and deletes it in order to test the functionality of players methods and check for memory leaks.

### integration testing

Client was tested by using it along side servers in the thayer collection.
Server was tested by using it along side the client in the thayer collection.

### system testing

Client and server were tested together by running the game and pushing it to edge cases.

---

## Limitations
In theory if there are too many piles for amount of spaces on the map, the gold with stop looping to put piles.

