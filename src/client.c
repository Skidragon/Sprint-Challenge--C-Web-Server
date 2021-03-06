#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lib.h"

#define BUFSIZE 4096 // max number of bytes we can get at once

/**
 * Struct to hold all three pieces of a URL
 */
typedef struct urlinfo_t {
  char *hostname;
  char *port;
  char *path;
} urlinfo_t;

/**
 * Tokenize the given URL into hostname, path, and port.
 *
 * url: The input URL to parse.
 *
 * Store hostname, path, and port in a urlinfo_t struct and return the struct.
*/
urlinfo_t *parse_url(char *url)
{
  // copy the input URL so as not to mutate the original
  char *hostname = strdup(url);
  char *port;
  char *path;
  
  char *end = strchr(hostname, ':');

  port = end + 1;

  *end = '\0';
  end = strchr(port, '/');

  path = end + 1;

  *end = '\0';
  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));
  
  urlinfo->hostname = hostname;
  urlinfo->port = port;
  urlinfo->path = path;

  printf("Hostname: %s\n Port: %s\n Path: %s\n", hostname, port, path);
  
  return urlinfo;
}
/**
 * Constructs and sends an HTTP request
 *
 * fd:
 * hostname: The hostname string.
 * port:     The port string.
 * path:     The path string.
 *
 * Return the value from the send() function.
*/
int send_request(int fd, char *hostname, char *port, char *path)
{
  const int max_request_size = 16384;
  char request[max_request_size];
  int rv;

  int request_length = sprintf(request,
    "GET /%s HTTP/1.1"
    "Host: %s:%s"
    "Connection: close",
    path,
    hostname,
    port
  );
  printf("request sent");
  send(fd, request, request_length, 0);

  return 0;
}

int main(int argc, char *argv[])
{  
  int sockfd, numbytes;  
  char buf[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }

  printf("%s\n", argv[1]);

  urlinfo_t *urlinfo = parse_url(argv[1]);

  int fd = get_socket(urlinfo->hostname, urlinfo->port);
  
  send_request(fd, urlinfo->hostname, urlinfo->port, urlinfo->path);

  while ((numbytes = recv(fd, buf, BUFSIZE - 1, 0)) > 0) {
    printf("%s\n", buf);
  }

  close(fd);

  if (urlinfo != NULL) {
    free(urlinfo);
  }
  /*
    1. Parse the input URL
    2. Initialize a socket
    3. Call send_request to construct the request and send it
    4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
    5. Clean up any allocated memory and open file descriptors.
  */

  return 0;
}
