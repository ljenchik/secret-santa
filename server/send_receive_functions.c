#include "send_receive_functions.h"

// Receives data from client with message length and error
void receive_from_client(int client_socket, void *buffer, int size_to_receive, char *error)
{
  int received_bytes = 0;
  do
  {
    int read_bytes = recv(client_socket, buffer + received_bytes, size_to_receive, 0);
    // EAGAIN non-blocking
    if (read_bytes == -1)
    {
      perror(error);
      exit(EXIT_FAILURE);
    }
    else if (read > 0)
    {
      received_bytes += read_bytes;
      size_to_receive -= read_bytes;
    }
    else
    {
      sleep(1);
    }

  } while (size_to_receive > 0);
}

// Sends data from server with message length and error
void send_to_client(int client_socket, void *buffer, int size_to_send, char *error)
{
  int sent = 0;
  do
  {
    int sent_bytes = send(client_socket, buffer + sent, size_to_send, 0);
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
