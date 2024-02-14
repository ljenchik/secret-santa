#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <pthread.h>
#include "linked_list_functions.h"
#include "send_receive_functions.h"

#define PORT 12345
#define BUFFER_SIZE 100
#define MAX_CLIENTS 100

Client_list *head;
pthread_mutex_t mutex;
char buffer[BUFFER_SIZE];
int number_of_clients = 0;

void *handle_client_thread(void *arg)
{
  int client_socket = *((int *)arg);
  // Creating a new client
  Client_list *new_client = create_client(client_socket);
  // Adding a new client to a linked list, protecting client's name with mutex while adding
  pthread_mutex_lock(&mutex);
  head = add_client(head, new_client);
  number_of_clients++;
  pthread_mutex_unlock(&mutex);

  int message = 0;
  // Continuously looping trying to receive a message from a client
  while (1)
  {
    // Receiving message from a client
    int received = recv(client_socket, &message, sizeof(message), 0);
    // Checking if server received any bytes
    if (received > 0)
    {
      receive_from_client(client_socket, &message + received, sizeof(message) - received, "Invalid client message");
      if (message == 0)
      {
        // Receives zero from a client as a signal to connect
        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, "Connection to the server successfull");
        // Sends a successful connection message to a client
        send_to_client(client_socket, buffer, strlen(buffer), 0);
      }
      else if (message == 1)
      {
        // Receiving name length from a client
        int name_len;
        receive_from_client(client_socket, &name_len, sizeof(name_len), "Failed to read name length");
        char *name = malloc(name_len);
        if (name == NULL)
        {
          perror("Failed to allocate memory for client's name");
          continue;
        }
        // Receiving name from a client
        receive_from_client(client_socket, name, name_len, "Falied to read name");
        new_client->client.name = name;
        printf("New client registered, name: %s\n", name);
      }
      else if (message == 2 && number_of_clients > 1)
      {
        printf("===========================DRAW============================\n");
        // Shiftin a linked list and adding giftees' names to Client_list nodes
        head = shift_list(head);
        print_clients_and_giftees(head);
        Client_list *temp = head;
        while (temp != NULL)
        {
          memset(buffer, 0, BUFFER_SIZE);
          strcpy(buffer, temp->giftee.name);
          // Sending giftee's name to their Secret Santa
          send_to_client(temp->client.sd, buffer, strlen(buffer), "Sending giftee name failed");
          temp = temp->next;
        }
      }
    }
  }
  sleep(2);
  close(client_socket);
  pthread_exit(NULL);
}

int main(void)
{
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;
  socklen_t client_addr_len = sizeof(client_address);
  socklen_t server_addr_len = sizeof(server_address);

  pthread_mutex_init(&mutex, NULL);

  pthread_t client_thread;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  server_address.sin_addr.s_addr = INADDR_ANY;
  server_address.sin_family = AF_INET;
  server_address.sin_port = htons(PORT);

  // Bind server socket to address
  if (bind(server_socket, (struct sockaddr *)&server_address, server_addr_len) == -1)
  {
    perror("Socket binding failed");
    exit(EXIT_FAILURE);
  }
  // Prepare the server for incoming client requests
  // 10 is the maximum length for the queue of pending connections
  if (listen(server_socket, 10) == -1)
  {
    perror("Listening failed");
    exit(EXIT_FAILURE);
  }

  printf("Server listening on port %d...\n", ntohs(server_address.sin_port));
  Client_list *head = NULL;
  Client_list *current = head;

  while (1)
  {
    // Accepting a new connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_addr_len)) > 0)
    {
      printf("===========================================================\n");
      printf("New connection, socket is %d, IP is : %s, port : %d\n", client_socket,
             inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

      // Creating a new thread to handle client communication
      int *client_socket_ptr = malloc(sizeof(int));
      if (client_socket_ptr == NULL)
      {
        perror("Failed to allocate memory for client socket");
        continue;
      }
      *client_socket_ptr = client_socket;

      if (pthread_create(&client_thread, NULL, handle_client_thread, (void *)client_socket_ptr) != 0)
      {
        perror("Failed to create thread");
        close(client_socket);
        free(client_socket_ptr);
      }
    }
    sleep(2);
  }
  // Joining all thtreads to main
  for (int i = 0; i < number_of_clients; i++)
  {
    pthread_join(client_thread, NULL);
  }
  close(server_socket);
  return 0;
}