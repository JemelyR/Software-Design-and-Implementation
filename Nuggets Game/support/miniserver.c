/* miniserver.c - a miniserver
*
* Author: Daniel Ha, 2023
* 
* miniserver.c
* purpose: a lil server that prints the messages it gets
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "message.h"
#include <string.h>

//local helper function
static bool handleMessage(void* arg, const addr_t from, const char* message);
static bool sendMap(void* arg);

//main
//program executes from here
int main(){
	//initialize server
	int address = message_init(stderr);

	//start message loop
	message_loop(NULL, 0, NULL, sendMap, handleMessage);
	

	//close server
	message_done();
}


/**************** handleMessage ****************/
/* Datagram received; print it.
 * We ignore 'arg' here.
 * Return true if any fatal error.
 */
static bool
handleMessage(void* arg, const addr_t from, const char* message)
{
	char* restrict string = message;
	char* token = strtok(string, " ");
	if ((strcmp(message, "SPECTATE") == 0) || strcmp(token, "PLAYER") == 0){
		
		message_send(from, message);
	}
	fflush(stdout);
	return false;
}

/********* send map **************/
/* sends the map to the client
 * 
 */

static bool sendMap(void* arg){
	addr_t* clienty = arg;
	const char* massage = "MAP ";
	message_send(*clienty, massage);
	return false;
}
