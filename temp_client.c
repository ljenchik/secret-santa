// Create a server Client application
// TCP connection
// The client “register” to the server
// The server every 5 minutes sends the current time
// the client print the current time and “sleep” again

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

int main()
{
  int ser_sd = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  int zero = 0;

  socklen_t ser_addr_len = sizeof(server_addr);

  memset(&server_addr, 0, ser_addr_len);
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  inet_pton(AF_INET, "127.0.0.1", &server_addr.sin_addr);

  connect(ser_sd, (struct sockaddr *)&server_addr, ser_addr_len);

  while (1)
  {
    memset(buffer, 0, sizeof(buffer));
    if (send(ser_sd, &zero, sizeof(zero), 0) == -1)
    {
      perror("Sending data failed");
      exit(EXIT_FAILURE);
    }
    printf("Sent to server: %d \n", zero);

    sleep(5);

    recv(ser_sd, buffer, sizeof(buffer), 0);
    printf("Received from server: %s \n", buffer);
  }

  close(ser_sd);

  return 0;
}
