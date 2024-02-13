#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <errno.h>
#include "linked_list_functions.c"
#include "send_receive_functions.c"

#define PORT 12345
#define BUFFER_SIZE 100
#define MAX_CLIENTS 100

// Structure for data when clients is rending a register request
typedef struct
{
  int number;
  char name[BUFFER_SIZE];
} data;

int main(void)
{
  int server_socket, client_socket;
  struct sockaddr_in server_address, client_address;

  socklen_t client_addr_len = sizeof(client_address);
  socklen_t server_addr_len = sizeof(server_address);

  char buffer[BUFFER_SIZE];

  int received_zero, received_one, received_two;

  data client_data;

  int number_of_clients = 0;

  server_socket = socket(AF_INET, SOCK_STREAM, 0);

  // Sets the socket to non-blocking mode
  //  It retrieves the current file status flags of the socket using fcntl(server_socket, F_GETFL, 0)
  //  and then sets the O_NONBLOCK flag by performing a bitwise OR operation with the existing flags.
  //  The F_SETFL command is used to set the file status flags.

  int status = fcntl(server_socket, F_SETFL, fcntl(server_socket, F_GETFL, 0) | O_NONBLOCK);

  if (status == -1)
  {
    perror("calling fcntl");
  }

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

  while (1)
  {
    // Accepting a new connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_addr_len)) > 0)
    {

      printf("=================================================\n");
      printf("New connection, socket is %d, IP is : %s, port : %d\n", client_socket,
             inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

      // Creating a client to add to a linked list
      Client_list *new_client = create_client(client_socket);
      head = add_client(head, new_client);
      number_of_clients++;
    }

    // Trying to get message from all clients to start the draw
    Client_list *current = head;
    while (current != NULL)
    {
      int message = 0;
      int client_socket = current->client.sd;
      int received = recv(current->client.sd, &message, sizeof(message), 0);
      if (received > 0)
      {
        receive_from_client(client_socket, &message + received, sizeof(message) - received, "Invalid client message");
        if (message == 2)
        {
          // Begin draw
          if (number_of_clients > 1 && number_of_clients <= MAX_CLIENTS)
          {
            printf("============== DRAW =================\n");
            head = shift_list(head);
            print_clients_and_giftees(head);

            Client_list *temp = head;
            while (temp != NULL)
            {
              memset(buffer, 0, BUFFER_SIZE);
              strcpy(buffer, temp->giftee.name);
              send_to_client(temp->client.sd, buffer, strlen(buffer), "Sending giftee name failed");
              temp = temp->next;
            }
          }
        }
        else if (message == 0)
        {
          // Receives zero from a client as a signal to connect
          memset(buffer, 0, BUFFER_SIZE);
          strcpy(buffer, "Connection to the server successfull");
          // Sends a successful connection message to a client
          send_to_client(client_socket, buffer, strlen(buffer), 0);
        }
        else if (message == 1)
        {
          int name_len;
          receive_from_client(client_socket, &name_len, sizeof(name_len), "Failed to read name length");
          char *name = malloc(name_len);
          receive_from_client(client_socket, name, name_len, "Falied to read name");
          current->client.name = name;
          printf("New client registered, name: %s\n", name);
        }
      }

      current = current->next;
    }
    sleep(1);
  }
  close(server_socket);
  return 0;
}