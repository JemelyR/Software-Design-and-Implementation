# CS50 Nuggets
## DR.JR, Winter 2023

### client

**client** is called from the command line. with the following format:

``./client <hostname> <port> <optional: username>``

### Usage

The client uses functions from message.h and makes use of the ncurses library. It is used to connect to a host and to display the game running in the server. Below are the functions used in client.c

```c
int main(int argc, char* argv[]);
char initializer(int argc, char* argv[]);
static void clientHandler(void* arg, const addr_t server);
static bool okay(void* arg, const addr_t server, const char* message);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleGridMessage(void* arg, const addr_t from, const char* message);
static bool handleTimeout(void* arg)
```

### Implementation

The `**main**` function accepts the commands from user input, verifies if the input is valid for a client, determines if the client is a spectator or a player, and runs clientHandler, used as a gameplay loop, to continuously receive display information from the server that is hosting the game. It makes use of functions such as `initializer` and `clientHandler`.  

The `**initializer**` function checks the validity of user command inputs and initializes the message module to set up communications between server and client. It returns a `char` that represents the client's identity  ('p' for player, 's' for spectator, 'f' invalid user input) and tells if the user input was invalid.

`**clientHandler**`is called by `main` in message_loop to start the display for the user, check dimensions of screen and grid, runs the gameplay loop, and determine when to end the game display. The `ncurses` library is used for displaying, and the message module is used to send to and receive messages from the server. 

The `**handleMessage**` function receives messages from server and interprets it. If a message from server begins with "QUIT," "GOLD," or "DISPLAY," handlemessage takes the information from the message after the identifying word and handles it accordingly. 

The `**handleGridMessage**` function takes a string message from server and determines the dimensions of it. This info is used later to tell the user that they have to increase the size of their window if too small.



### Assumptions
Client does not send names to server that are too long (longer than 30 characters).


### Files

* `Makefile` - a compilation procedure
* `client.c` - the implementation
* `client.h` - module for gold

### Compilation

To compile use "make all" when in the top directory or "make client" when in client directory.

### Testing
Tested using server to client connections.


