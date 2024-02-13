#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// Sends data from client with message length and error
void send_to_server(int server_socket, void *buffer, int size_to_send, char *error)
{
  int sent = 0;
  do
  {
    int sent_bytes = send(server_socket, buffer + sent, size_to_send, 0);
    if (sent_bytes == -1)
    {
      perror(error);
      exit(EXIT_FAILURE);
    }
    else if (sent_bytes > 0)
    {
      sent += sent_bytes;
      size_to_send -= sent_bytes;
    }
    else
    {
      sleep(1);
    }

  } while (size_to_send > 0);
}
