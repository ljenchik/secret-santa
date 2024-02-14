#ifndef SEND_RECEIVE_FUNCTIONS
#define SEND_RECEIVE_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>

// Receives data from client with message length and error
void receive_from_client(int client_socket, void *buffer, int size_to_receive, char *error);

// Sends data from server with message length and error
void send_to_client(int client_socket, void *buffer, int size_to_send, char *error);

#endif