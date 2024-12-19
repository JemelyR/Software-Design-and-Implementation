/*
 * client.h - a client for the Nuggets game
 * 
 * Author: Daniel Ha, 2023
 */

/************ initializer ************/
/* Takes user input from main() and initializes the client as a player or as a
 * spectator.
 *
 * Returns char based on status of the run:
 *   'f' - failure to initialize because user input is incorrect
 *   'p' - initialized a player
 *   's' - initialized a spectator
 */
char initializer(int argc, char* argv[]);

/************ clientHandler ************/
/* Uses ncurses to monitor & manage both user and server inputs.
 *
 * Caller provides:
 *   character pointer 'arg' for a player client/NULL 'arg' for a spectator 
 *   client
 *   valid server address
 * We guarantee:
 *   a game screen will be presented and updated & user inputs will be monitored
 *   and sent to the server as specified in specification documents.
 * 
 * no return value.
 */
static void clientHandler(void* arg, const addr_t server);
