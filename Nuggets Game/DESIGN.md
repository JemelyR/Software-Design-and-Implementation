# CS50 Nuggets
## Design Spec
### Dr. Jr, Winter, 2023

According to the Requirements Spec, the Nuggets game requires two standalone programs: a client and a server.
Our design also includes `grid`, `gold`, and `players` modules.
We describe each program and module separately.
We do not describe the `support` library nor the modules that enable features that go beyond the spec.
We avoid repeating information that is provided in the requirements spec.

## Client

The *client* acts in one of two modes:

 1. *spectator*, the passive spectator mode described in the requirements spec.
 2. *player*, the interactive game-playing mode described in the requirements spec.

### User interface

See the requirements spec for both the command-line and interactive UI.

### Inputs and outputs

**Inputs:**
The client accepts keystrokes as input. 
The following characters are considered "valid":

 * `Q` quit the game. - for spectator and player
 * `h` move left, if possible
 * `l` move right, if possible
 * `j` move down, if possible
 * `k` move up	, if possible
 * `y` move diagonally up and left, if possible
 * `u` move diagonally up and right, if possible
 * `b` move diagonally down and left, if possible
 * `n` move diagonally down and right, if possible

Any keystroke outside of that list is considered "invalid."

**Outputs:**
The client outputs to a stdout display and to a log file.
The display consists of *NR+1* rows and *NC* columns.
The top row displays the game's status, while the remaining rows display the game's map.
The map characters are:

 * ` ` solid rock - interstitial space outside rooms
 * `-` a horizontal boundary
 * `|` a vertical boundary
 * `+` a corner boundary
 * `.` an empty room spot
 * `#` an empty passage spot
 * `@` the player
 * `A`-`Z` another player
 * `*` a pile of gold

Errors detected while the client runs are written to log files.
In addition to errors, the outputs to the log file include all messages sent and received by the client.

### Functional decomposition into modules

1. *initializer*, which parses arguments and initializes other modules 
2. *clientHandler*, which listens for and acts on keyboard input from the user and messages from the server
 
### Pseudo code for logic/algorithmic flow

```
 	  execute from a command line per the requirement spec
    use initializer to parse the command line, validate parameters
    use initializer to initialize the 'message' module
    if playername provided
	    send PLAY message to server
	    call message_loop() to check response
	    if response is an OK message
		    use clientHandler to run the gameplay loop
    if no playername provided
	    send SPECTATE message to server
	    use clientHandler to run the spectatorship loop
    when QUIT message received, clean up
```

### Major data structures

We plan to use no unique data structures in the client.

---

## Server
### User interface

See the requirements spec for the command-line interface.
Otherwise, there is no interaction with the user.

### Inputs and outputs

**Command Line Arguments:** 
In the command line, server accepts either _map.txt_ and _seed_ or just _map.txt_.
_map.txt_ is the map file, i.e. a text file that contains a game map in string form, with NR lines with NC characters per line. 
_seed_ is a random seed, and an optional argument. 
If _seed_ is not provided, the server uses `srand(getpid())` to produce random behavior.

**Inputs:**
No inputs.

**Output:** 
A run of server writes to standard error, including:
* messages sent by the server,
* message received by the server, and 
* any errors detected while the server was run.

### Functional decomposition into modules

We anticipate the following modules or functions:

1. *initializer*, which parses arguments and initializes other modules
2. *messageHandler*, which listens for messages from clients and responds accordingly
3. *gameOver*, which sends messages and closes the server at the end of the game

And some helper modules that provide data structures:

1. *grid*, a module providing the grid data structure to represent the game map, and the function to display that map to clients, taking visibility into account.
2. *gold*, a module providing the gold data structure to represent all the gold in the game, and functions to modify that structure.
3. *players*, a module providing the players data structure to represent a list of the game's players, and functions to set and get player parameters.

### Pseudo code for logic/algorithmic flow

The server will run as follows:
```
 	  execute from a command line per the requirement spec
    use initializer to parse the command line, validate parameters
    use initializer to set up data structures and initialize the 'message' module
    print the port number on which we wait
    use messageHandler to call message_loop() to await clients
    call gameOver() to inform all clients the game has ended
    clean up
```

#### initialize

The `initialize` function fulfills the role of *initializer*. 
Pseudocode:
```
	  validate map.txt
	    if map.txt cannot be validated, print an error message to stderr and exit non-zero
	  check for seed
	  if seed is present, validate seed:
		  if seed cannot be validated, print error message to stderr and exit non-zero
	  if seed is absent, generate a random seed and set seed to that
    call grid_new() to initialize and save grid data structure
	  call gold_new() to initialize and save gold data structure
    call players_new() to initialize and save an empty players data structure
    if any of the data structure initializations returns null, print an error message to stderr and exit non-zero
```

#### handleMessage

This functions fulfills the role of *messageHandler* and acts as handleMessage in message_loop.
Pseudocode:
```
	check message type and handle accordingly
	if the message is PLAY
		if there are already a maximum number of players, respond with a meaningful QUIT message
		otherwise
      call players_add() to add the client as a player and respond with OK
      send the client GRID and GOLD messages
	if the message is SPECTATE
		if the server has a spectator, send them a QUIT message
		send the client GRID and GOLD messages
	if the message is KEY
		if the key is Q, respond with a QUIT message and call players_remove()
		otherwise, check using grid and players data structures what happens when the corresponding player moves in the given direction
		  if the direction is wall, ignore it
			otherwise
				call players_move()
				if the direction is player, call players_move() for other player
        if the direction is gold, call gold_find() and send all clients GOLD messages
  send all clients DISPLAY messages
```

#### gameOver

```
	  initialize gameover string
    for each player in the player list
	    add the player's name, corresponding character, and purse value to the gameover string
    for client in clients
	    send GAMEOVER message to client containing the gameover string
		  send QUIT message to client informing them the game is over
```

### Major data structures

The data structures *grid*, *gold*, and *players* are all used in the server, and each is detailed in its corresponding module.

---

## Grid

The grid module contains functions pertaining to the game's map and its presentation.

### Functional decomposition

1. `grid_new` initializes a grid from a map file
2. `grid_get` returns the character type of a specified gridpoint in a grid
3. `grid_display` returns a string version of a grid with respect to a given visibility string

### Pseudo code for logic/algorithmic flow

#### grid_new

  Pseudocode:
```
    verify that given map file can be opened
	  if so,
		  save the contents of the map file as a string
		  return true
	  if not, return false
```

#### grid_get

  Pseudocode:
```
	  if the grid's map string is null, return null type
	  otherwise, 
		  if there is a player at the location of n, return their key-character
		  if there is gold at the location of n, return '*'
    otherwise, return the nth character of the map string
```

#### grid_display

  Pseudocode:
```
	  initialize the output map string to a string of spaces and newlines of the same size and format as the map string
	  for each character M in the map string
		  store the corresponding character in the visibility string as C
		  if C is '0', move on
		  if C is '1', change the corresponding character in the output map string to M
		  if C is '2' and M is '*' or a letter
        change the corresponding character in the output map string to '.'
	    if C is '2' and M is neither '*' nor a letter
        change the corresponding character in the output map string to M
    print the output map string to stdout	
```

### Major data structures

The grid module uses no non-standard major data structures.

## Gold

The gold module contains functions pertaining to managing all of the gold in the game.

### Functional decomposition

1. `gold_new` initializes a list of gold-piles and fills it with a random number of piles, distributed randomly through a map, whose values sum up to a total gold count
3. `gold_find` transfers the value of a gold-pile to a purse location
4. `gold_get` returns the gold value at a location in the map
5. `gold_getPurse` returns the gold value in a player's purse

### Pseudo code for logic/algorithmic flow

#### gold_new

  Pseudocode:
```
    initialize gold data structure
	  set the remaining gold count to the total gold count
    while the remaining gold count exceeds 0 and there are fewer than GoldMaxNumPiles piles
		  randomly generate a gold value from 1 to the remaining gold count - GoldMinNumPiles
		  randomly select a spot from the map
		  add a gold-pile to the list at the selected spot with the selected value
	  return the resulting gold data structure
```

#### gold_find

  Pseudocode:
```
	  validate the given player and location
	  if invalid, write an error message to stderr and return false
	  if valid, transfer the value of the gold-file at that location to the location of the player's purse
	  return true
```

### Major data structures

The key data structure `gold` uses is the _nugget set_, a linked list that operates similarly to standard set, but is indexed with integers and has functions to modify and delete existing nodes of the list.
The head of a nugget set is a 'special' node which contains meta information instead of being an item in the list.

## Players

The players module contains functions pertaining to the game's players and their locations on the grid.

### Functional decomposition

1. `players_new` initializes an empty list of players
2. `players_add` adds a player to the players-list
3. `players_move` changes a player's location
4. `players_remove` removes a player from the player-list

### Pseudo code for logic/algorithmic flow

#### players_add

  Pseudocode:
```
	  create a player node
	  if the player node is null, write an error message to stderr and return null
	  otherwise
      add the player node to the players list
      update the character at the list's head to ensure the player node will not be repeated
    return the list's head
```

#### players_move

  Pseudocode:
```
	  verify that the player-list is not null and contains the given player
	  if not, return false
	  access the given player in the list and change their location to the given location
	  return true
```

#### players_remove

  Pseudocode:
```
	  verify that the player-list is not null and contains the given player
	  if so, set the player's location to -999 off the map
```

### Major data structures

The key data structure `players` uses is the _nugget set_, which is detailed in the data structures section of `gold`.
