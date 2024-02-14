#ifndef LINKED_LIST_FUNCTIONS
#define LINKED_LIST_FUNCTIONS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct client
{
  char *name;
  int sd;
} Client;

// Linked list to store clients' names and sockets' ids
typedef struct client_list
{
  Client client;
  Client giftee;
  struct client_list *next;
} Client_list;

// Creating a new vlient
Client_list *create_client(int sd);

// Printing client's info
void print_client(Client *client);

// Printing all clients
void print_clients(Client_list *head);

// Adding client to a linked list (at the beginning) and returning a new head
Client_list *add_client(Client_list *head, Client_list *new_client);

Client_list *get_last(Client_list *head);

// Shifting a linked list
Client_list *shift_list(Client_list *head);

// Printing all clients with giftees
void print_clients_and_giftees(Client_list *head);

#endif