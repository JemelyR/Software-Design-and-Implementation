/* 
 * client.c - a client for Nuggets
 * Author: Daniel Ha, 2023
 * 
 * purpose: This program is used by someone trying to spectate or join a Nuggets game.
 * use: "./client <hostname/IP> <port number> <username>(optional)"
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include "message.h"
#include <ncurses.h>
#include <unistd.h>
#include "client.h"


/* LOCAL HELPER FUNCTION PROTOTYPES */
static bool okay(void* arg, const addr_t server, const char* message);
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool handleGridMessage(void* arg, const addr_t from, const char* message);
static bool handleTimeout(void* arg);


/* MAIN */
/*************** main() ***************/
/* Executes program, calling initializer and client-handler modules.
 *
 * If an issue is encountered during initialization, exits 1.
 * Otherwise, returns 0.
 */
int main(int argc, char* argv[]){

  //initialize the client
  //determine if its a player or spectator
  char role = initializer(argc, argv);
  
  //if arguments invalid, exit (it will have printed error message)
  if (role == 'f'){
    exit(1);
  }

  //**if arguments valid**//
  // //initialize address of server from arguments
  addr_t serverAddress;
  message_setAddr(argv[1], argv[2], &serverAddress);

  //if it's a PLAYER
  if (role == 'p'){
    char message[36] = "PLAY "; // player name capped at 30 characters

    //get full username, taking into consideration spaces 
    char username[30] = "";
    int i = 3;
    while (i < argc){
      if (i == argc - 1){
        strcat(username, argv[i]);
        break;
      }
      strcat(username, argv[i]);
      strcat(username, " ");
      i++;
    }

    //make sure the username is not too long!
    if (strlen(username)>=31){
      printf("username too long! (max characters is 30) \n");
      exit(1);
    }

    strncat(message, username, 30);
    message_send(serverAddress, message); //inform server of player

    //start message loop, if the answer is "OK", then the game loop will begin
    char identity = '~';
    if (message_loop(&identity, 0.0, NULL, NULL, okay) && identity != '~') {
      clientHandler(&identity, serverAddress); //game loop
    }

  //if it's a SPECTATOR
  } else {
    message_send(serverAddress, "SPECTATE"); //inform server of spectator
    clientHandler(NULL, serverAddress);
  }

  message_done();
  return 0;
}


/* GLOBAL FUNCTIONS */

/*************** initializer() ***************/
/* see client.h for description */
char initializer(int argc, char* argv[]){
  //***check if input is invalid***//
    //make sure command line input is formatted properly
  if(argc < 3){
    fprintf(stderr, "Error: correct usage: ./client <host> <port> <optional: username>\n");
    return 'f';
  }

    //make sure port and host are valid
  addr_t serverAddress;
  if (message_setAddr(argv[1], argv[2], &serverAddress) == false){
    fprintf(stderr, "Error: invalid host or port \n");
    message_done();
    return 'f';
  }

  //***if input is valid***//
    //initialize message module for client
  if (message_init(NULL) == 0) {
    printf("failed initializing \n");
    return 'f';
  }

    //determine if user is a player or spectator
  if(argc >= 4){
    return 'p';
  }
  else{
    return 's';
  }
}

/*************** clientHandler() ***************/
/* see client.h for description */
static void clientHandler(void* arg, const addr_t server){
  // cast & save identity character
  char* identityPointer = arg;
  char identity;
  if (arg != NULL) {
    identity = *identityPointer;
  }

  //initialize screen
  initscr();
  cbreak();
  noecho();
  nodelay(stdscr, true);
  start_color();

  //get dimensions of screen
  int sy, sx;
  getmaxyx(stdscr, sy, sx);
  //get dimensions of grid
  int dim[2] = {0, 0};
  message_loop(dim, 0.0, NULL, NULL, handleGridMessage);
  int gy = dim[0] + 1;
  int gx = dim[1];
  
  //if the dimensions of the map are larger than the actual window:
  while (sy < gy || sx < gx) {
    mvprintw(0, 0, "Sorry, you can't play the game until ");
    printw("your screen is at least %d characters wide and %d tall.", gx, gy);
    printw(" (Currently, it is %d wide and %d tall.)\n", sx, sy);
    sleep(1);
    endwin();
    refresh();
    getmaxyx(stdscr, sy, sx);
  }
  char display[(sx*sy + 1)];
  
  // Gameplay Loop: runs until QUIT message received
  bool running = true;
  while (running) {
    // if input received, send message
    char c = getch();
      // player input management
    if (arg != NULL && c != ERR) {
      char message[8];
      sprintf(message, "KEY %c %c", c, identity);
      message_send(server, message);
      // spectator input management
    } else if (arg == NULL && c == 'Q') {
      message_send(server, "QUIT");
    }

    // update display (timeout allows ~100 key-presses per second)
    strncpy(display, "", (sx*sy + 1));
    running = message_loop(display, 0.01, handleTimeout, NULL, handleMessage);
    // check for exit condition
    if (strncmp(display, "QUIT ", 5) == 0) {
      running = false;
    }

    refresh();
  }

  //end screen if not already ended
  endwin();
  
  // print QUIT message to standard output
  for (int i = 5; i < strlen(display); i++) { putchar(display[i]); }
  putchar('\n');
}


/* LOCAL FUNCTIONS */

/*** okay() ***/
/* Listen for and handle an OK message from the server.
 * Returns true once an OK or QUIT message is received from the server.
 */
static bool okay(void* arg, const addr_t server, const char* message) {
  // if OK message received, accept its character key as identity
  if (strncmp(message, "OK ", 3) == 0) {
    char* identity = arg;
    *identity = message[3];
  
  // otherwise, return false (with explanation in case of QUIT message)
  } else {
    if (strncmp(message, "QUIT ", 5) == 0) {
      // print explanation
      for (int i = 5; i < strlen(message); i++) { printf("%c", message[i]); }
      printf("\n");
    }
  }
  return true; //once OK (accepted by server) or QUIT (rejected by server)
}

/*** handleMessage() ***/
/* Handle all server messages after the intial OK and GRID. This includes:
 *   - Displaying updates to the gold-report on the top row of the screen (GOLD)
 *   - Updating the map display on the screen (DISPLAY)
 *   - Handling the GAMEOVER condition by closing the screen and printing the 
 *     end-of-game report
 *   - Giving the client-handler loop the cue to quit (QUIT)
 *
 * Returns true once any messages of the above types are received.
 */
static bool handleMessage(void* arg, const addr_t from, const char* message) {
  char* display = arg;

  // QUIT message is the only return-false condition
  if (strncmp(message, "QUIT ", 5) == 0) {
    strcpy(display, message);
  // but GAMEOVER message closes the game screen
  } else if (strncmp(message, "GAMEOVER\n", 9) == 0) {
    endwin();
    for (int i = 9; i < strlen(message); i++) {
      putchar(message[i]);
    }
  }
  
  // Otherwise, will return true at end
  // // GOLD message adjusts top-line printout
  if (strncmp(message, "GOLD ", 5) == 0) {
    for (int i = 5; i < strlen(message); i++) { 
      display[i-5] = message[i];
    }
    mvprintw(0, 0, "%s\n", display);

  // // DISPLAY message adjusts map printout
  } else if (strncmp(message, "DISPLAY\n", 8) == 0) {
    for (int i = 8; i < strlen(message); i++) { 
      display[i-8] =  message[i]; 
    }
    mvprintw(1, 0, "%s\n", display);
  }

  return true;
}

/*** handleGridMessage() ***/
/* Listen for and parse the initial GRID message from the server into a 
 * row-by-column integer pair.
 *
 * Returns true when the GRID message is received.
 */
static bool
handleGridMessage(void* arg, const addr_t from, const char* message)
{
  // check to ensure message is of correct format
  if (strncmp(message, "GRID ", 5) != 0) {
    return false;
  }

  // parse message
  int row, col;
  sscanf(message, "GRID %d rows, %d cols", &row, &col);
  
  int* dimensions = arg;
  dimensions[0] = row;
  dimensions[1] = col;

  return true;
}

/*** handleTimeout() ***/
/* Marks wait-too-long as a valid return-true condition */
static bool 
handleTimeout(void* arg) { 
  return true; 
}
