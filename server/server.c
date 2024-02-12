#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/select.h>
#include <sys/types.h>
#include <pthread.h>
#include "linked_list_functions.c"
// #include "linked_list_functions.h"

#define PORT 12345
#define BUFFER_SIZE 100

// Structure for data when clients is rending a register request
typedef struct
{
  int number;
  char name[BUFFER_SIZE];
} data;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

int main(void)
{
  int server_socket, client_socket, i;
  struct sockaddr_in server_address, client_address;

  socklen_t client_addr_len = sizeof(client_address);
  socklen_t server_addr_len = sizeof(server_address);

  char buffer[BUFFER_SIZE];
  int received_zero;
  int received_one;
  data client_data;
  int number_of_clients = 0;
  int pid;

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

  while (1)
  {
    // Accepting a new connection
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_address, &client_addr_len)) < 0)
    {
      perror("Accept failed");
    };

    printf("====================================================================\n");
    printf("New connection, socket is %d, IP is : %s, port : %d\n", client_socket,
           inet_ntoa(client_address.sin_addr), ntohs(client_address.sin_port));

    // Creating child process for each client
    // printf("head before clients %s \n", head->client.name);
    // pid = fork();
    // i++;
    // if (pid == 0)
    // {
    // close(server_socket);

    // Receives zero from a client as a signal to connect
    if (recv(client_socket, &received_zero, sizeof(received_zero), 0) == -1)
    {
      perror("Receiving 0 failed");
      exit(EXIT_FAILURE);
    }
    else
    {
      memset(buffer, 0, BUFFER_SIZE);
      strcpy(buffer, "Connection to the server successfull\n");
      send(client_socket, buffer, strlen(buffer), 0);
    }

    if (recv(client_socket, &client_data, sizeof(client_data), 0) == -1)
    {
      perror("Receiving client's data failed");
      exit(EXIT_FAILURE);
    }
    else
    {
      // Client sends 1 as a signal to register
      if (client_data.number == 1)
      {
        memset(buffer, 0, BUFFER_SIZE);
        strcpy(buffer, "Registration with the server successfull\n");
        send(client_socket, buffer, strlen(buffer), 0);

        // Creating a client to add to a linked list
        Client_list *new_client = create_client(client_data.name, client_socket);
        pthread_mutex_lock(&mutex);
        head = add_client(head, new_client);
        pthread_mutex_unlock(&mutex);
        printf("%s is registered \n", client_data.name);
        number_of_clients++;

        if (number_of_clients == 4)
        {
          head = shift_list(head);
          printf("====================================================================\n");
          print_clients_and_giftees(head);

          Client_list *current = head;

          while (current != NULL)
          {
            memset(buffer, 0, BUFFER_SIZE);
            strcpy(buffer, current->giftee.name);
            if (send(current->client.sd, buffer, strlen(buffer), 0) == -1)
            {
              perror("Sending giftee name failed");
              exit(EXIT_FAILURE);
            }
            current = current->next;
          }
        }
      }
      else
      {
        perror("Registration error");
        exit(EXIT_FAILURE);
      }

      //   close(client_socket);
      // exit(0);
      // }
    }
  }
  // wait(NULL);
  close(server_socket);
  return 0;
}
