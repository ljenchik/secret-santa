# Feedback on Olena’s Secret Santa Project

16/02/2024
by Matthew Crowson

## Pointer arithmetic with void pointers

```
server/send_receive_functions.c:9:49: warning: arithmetic on a pointer to void is a GNU extension [-Wgnu-pointer-arith]
    int read_bytes = recv(client_socket, buffer + received_bytes, size_to_receive, 0);
                                         ~~~~~~ ^
server/send_receive_functions.c:34:49: warning: arithmetic on a pointer to void is a GNU extension [-Wgnu-pointer-arith]
    int sent_bytes = send(client_socket, buffer + sent, size_to_send, 0);
                                         ~~~~~~ ^
```

See [c - void \* arithmetic - Stack Overflow](https://stackoverflow.com/questions/3922958/void-arithmetic) for an explanation.

## Preprocessor Directives

Good formatting and use of preprocessor directives.
In particular, great use of `#define` to avoid magic numbers.

## Thread-safety

Good to see that you're thinking about thread safety, by using a mutex to control access to the client list.

Move buffer into `handle_client_thread()`.\

## Networking

Very sensible choices regarding socket set up, using TCP, IPv4, and remembering to use `htons()` to keep code portable across devices.

## Checking for null pointers

```C
int *client_socket_ptr = malloc(sizeof(int));

if (client_socket_ptr == NULL)
{
  perror("Failed to allocate memory for client socket");
  continue;
}
```

Great job remembering to do this. An important habit to get into, well done.

## Remembering to free malloced memory

```C
 if (pthread_create(&client_thread, NULL, handle_client_thread, (void *)client_socket_ptr) != 0)
      {
        perror("Failed to create thread");
        close(client_socket);
        free(client_socket_ptr);
      }
```

```C
char *name = malloc(name_len);
if (name == NULL)
{
  perror("Failed to allocate memory for client's name");
  continue;
}

```

The above looks good. Nice to see that you're remembering to close the socket and free the memory. **Remember to also free in client, when client disconnects.**

### Unreachable Code

```C
while (1){
  ...
}
```

It is often nicer to see some kind of **descriptive boolean variable** used as the loop condition, or in some cases a function call. However, the above can be okay in some situations.

You must however make sure you provide break statements for the loop, otherwise these loops will never be exited.

**main:** The below was a nice idea to make sure storage is reclaimed rom the threads.

```C
  // Joining all thtreads to main
  for (int i = 0; i < number_of_clients; i++)
  {
    pthread_join(client_thread, NULL);
  }
  close(server_socket);
  return 0;
```

**handle_client:** good idea to close socket, but seems unreachable.

```C
  sleep(2);
  close(client_socket);
  pthread_exit(NULL);
```

## More descriptive argument name

```C
void *handle_client_thread(void *arg)
```

```C
void *handle_client_thread(void *client_socket)
```

## Function Doc Comments

```C
// Adding client to a linked list (at the beginning) and returning a new head
Client_list *add_client(Client_list *head, Client_list *new_client)
```

Well done for commenting on functions, keep doing this.
The above answered a question without me having to figure out the implementation.
Below could be more descriptive, say what you mean by shifting a linked list.

```C
// Shifting a linked list
Client_list *shift_list(Client_list *head)
```

## Prefer safer string functions

```C
strcpy(buffer, "Registration to the server successful");
```

Use strncpy instead.

In general, it is preferred to use the _"n"_ versions of string functions if possible. It is just much more safer and robust, because you are making sure the strings don't overflow the buffer.
