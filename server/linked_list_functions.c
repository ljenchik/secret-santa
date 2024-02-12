#include <stdio.h>
#include <stdlib.h>
#include <string.h>
// #include "linked_list_functions.h"

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
Client_list *create_client(char *name, int sd)
{
  Client_list *new_client = malloc(sizeof(Client_list));
  new_client->client.name = malloc(strlen(name));
  strcpy(new_client->client.name, name);
  new_client->client.sd = sd;
  new_client->next = NULL;
  return new_client;
}

// Printing client's info
void print_client(Client *client)
{
  printf("%s\t   %d\n", client->name, client->sd);
  printf("\n");
}

// Printing all clients
void print_clients(Client_list *head)
{
  if (head == NULL)
  {
    printf("Error: invalid pointer \n");
  }
  else
  {
    Client_list *current = head;
    printf("Name\t   Socket\n");
    printf("\n");
    while (current->next != NULL)
    {
      print_client(&current->client);
      current = current->next;
    }
  }
}

// Adding client to a linked list (at the beginning) and returning a new head
Client_list *add_client(Client_list *head, Client_list *new_client)
{
  if (head == NULL && new_client == NULL)
  {
    printf("Error: invalid pointer, no client to add \n");
    return NULL;
  }
  if (head == NULL && new_client != NULL)
  {
    head = new_client;
    return head;
  }
  else
  {
    new_client->next = head;
    head = new_client;
    return head;
  }
}

Client_list *get_last(Client_list *head)
{
  if (head == NULL)
  {
    printf("List is empty\n");
    return NULL;
  }
  Client_list *current = head;
  while (current->next != NULL)
  {
    current = current->next;
  }
  return current;
}

// Shifting a linked list
Client_list *shift_list(Client_list *head)
{
  if (head == NULL)
  {
    printf("Error: invalid pointer or insufficient clients for shifting\n");
    return NULL;
  }
  Client_list *current = head;
  while (1)
  {
    if (current->next == NULL)
    {
      current->giftee = head->client;
      break;
    }
    else
    {
      current->giftee = current->next->client;
      current = current->next;
    }
  }
  return head;
}

// Printing all clients with giftees
void print_clients_and_giftees(Client_list *head)
{
  if (head == NULL)
  {
    printf("Error: invalid pointer \n");
  }
  else
  {
    Client_list *current = head;
    printf("Santa\tSocket\tGiftee\tSocket\n");
    printf("\n");
    while (current != NULL)
    {
      printf("%s\t%d\t%s\t%d\n", current->client.name, current->client.sd, current->giftee.name, current->giftee.sd);
      printf("\n");
      current = current->next;
    }
  }
}