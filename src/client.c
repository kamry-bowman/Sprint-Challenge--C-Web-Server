#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "lib.h"
#include <ctype.h>

#define BUFSIZE 4096 // max number of bytes we can get at once

/**
 * Struct to hold all three pieces of a URL
 */
typedef struct urlinfo_t {
  char *hostname;
  char *port;
  char *path;
} urlinfo_t;

#include <ctype.h>


/**
 * Tokenize the given URL into hostname, path, and port.
 *
 * url: The input URL to parse.
 *
 * Store hostname, path, and port in a urlinfo_t struct and return the struct.
*/
urlinfo_t *parse_url(char *url)
{


  // convert to lower case
  for(int i = 0; url[i]; i++){
    url[i] = tolower(url[i]);
  }

  // advance past http:// or https://
  char * strippedHttp = strstr(url, "http");
  if (strippedHttp) {
    if (strippedHttp[4] == 's') {
      strippedHttp += 1;
    }
    url = strippedHttp + 7;
  }
  char *hostname = strdup(url);
  char *port;
  char *path;

  urlinfo_t *urlinfo = malloc(sizeof(urlinfo_t));

  char *ptr;
  ptr = strchr(hostname, ':');
  if (ptr) {
    ptr[0] = 0;
    port = strdup(ptr + 1);
    ptr = strchr(port, '/');
  } else {
    port = strdup("80");
    ptr = strchr(hostname, '/');
  }
  if (ptr) {
    path = strdup(ptr);
    ptr[0] = 0;
  } else {
    path = strdup("/");
  }

  urlinfo->hostname=hostname;
  urlinfo->port=port;
  urlinfo->path=path;

  return urlinfo;
}

void free_urlinfo_t(urlinfo_t * urlinfo) {
    free(urlinfo->hostname);
    free(urlinfo->port);
    free(urlinfo->path);
    free(urlinfo);
}

/**
 * Constructs and sends an HTTP request
 *
 * fd:       The file descriptor of the connection.
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
  int req_len = snprintf(
    request, 
    max_request_size,
    "GET %s HTTP/1.1\n"
    "Host: %s:%s\n"
    "Connection: close\n"
    "\n",
    path,
    hostname,
    port
    );

  return send(fd, request, req_len, 0);
}

int main(int argc, char *argv[])
{  
  int sockfd, numbytes;  
  char buf[BUFSIZE];

  if (argc != 2) {
    fprintf(stderr,"usage: client HOSTNAME:PORT/PATH\n");
    exit(1);
  }

  urlinfo_t *parsed_url = parse_url(argv[1]);
  

  int fd = get_socket(parsed_url->hostname, parsed_url->port);

  // get fd
  int send_res = send_request(
    fd, 
    parsed_url->hostname,
    parsed_url->port,
    parsed_url->path);


  if (send_res > 0) {
    while ((numbytes = recv(fd, buf, BUFSIZE - 1, 0)) > 0) {
      // print the data we got back to stdout
      printf("%s\n", buf);
    }   
  }

  free_urlinfo_t(parsed_url);
  close(fd);
  /*
    1. Parse the input URL
    2. Initialize a socket by calling the `get_socket` function from lib.c
    3. Call `send_request` to construct the request and send it
    4. Call `recv` in a loop until there is no more data to receive from the server. Print the received response to stdout.
    5. Clean up any allocated memory and open file descriptors.
  */

  ///////////////////
  // IMPLEMENT ME! //
  ///////////////////

  return 0;
}
