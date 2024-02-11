#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>

#define PORT 12345
#define BUFFER_SIZE 100

typedef struct
{
  int number;
  char name[BUFFER_SIZE];
} data;

#include <stdio.h>
#include <stdlib.h>

int main()
{
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

  connect(ser_sd, (struct sockaddr *)&server_addr, ser_addr_len);

  if (send(ser_sd, &zero, sizeof(zero), 0) == -1)
  {
    perror("Sending data failed");
    exit(EXIT_FAILURE);
  }
  printf("Sent to server: %d \n", zero);

  sleep(5);

  client_data.number = 1;
  strcpy(client_data.name, "Ned");

  if (send(ser_sd, &client_data, sizeof(client_data), 0) == -1)
  {
    perror("Sending data failed");
    exit(EXIT_FAILURE);
  }
  printf("Sent to server: %d and %s \n", client_data.number, client_data.name);

  sleep(5);

  close(ser_sd);

  return 0;
}
