#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include "send_to_server.c"

#define PORT 12345
#define BUFFER_SIZE 100

typedef struct
{
  int number;
  int name_length;
  char name[BUFFER_SIZE];
} data;

int main(int argc, char *argv[])
{

  if (argc <= 1)
  {
    perror("Name was not provided");
    exit(EXIT_FAILURE);
  }

  char *name = argv[1];
  int draw = argc == 3;

  int ser_sd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  int zero = 0;

  data client_data;

  socklen_t ser_addr_len = sizeof(server_addr);

  memset(&server_addr, 0, ser_addr_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  if (connect(ser_sd, (struct sockaddr *)&server_addr, ser_addr_len) < 0)
  {
    perror("Connection failed");
    exit(EXIT_FAILURE);
  }

  // Sending zero to a server as a request to connect
  send_to_server(ser_sd, &zero, sizeof(zero), "Sending zero failed");
  printf("Sent to server: %d \n", zero);

  if (recv(ser_sd, &buffer, sizeof(buffer), 0) == -1)
  {
    perror("Receiving connection confirmation failed");
    exit(EXIT_FAILURE);
  }
  else
  {
    if (strcmp(buffer, "Connection to the server successfull") == 0)
    {
      // Printing "Connection to the server successfull\n"
      printf("%s \n", buffer);

      // Sending 1, length of name, name
      int message = 1;
      int name_len = strlen(name);
      send_to_server(ser_sd, &message, sizeof(message), "Sending one failed");
      send_to_server(ser_sd, &name_len, sizeof(name_len), "Sending name length failed");
      send_to_server(ser_sd, name, name_len, "Sending name failed");
      printf("Sent to server: %d, %s, %d \n", message, name, name_len);

      if (draw)
      {
        int draw = 2;
        send_to_server(ser_sd, &draw, sizeof(draw), "Sending draw failed");
      }

      memset(buffer, 0, BUFFER_SIZE);
      if (recv(ser_sd, &buffer, sizeof(buffer), 0) == -1)
      {
        perror("Receiving giftee failed");
        exit(EXIT_FAILURE);
      }
      // Printing giftee's name
      sleep(2);
      printf("%s, your giftee is %s", name, buffer);
      printf("\n");
    }
  }
  sleep(2);
  close(ser_sd);
  return 0;
}
