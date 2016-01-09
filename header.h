#ifndef HEADER_H_INCLUDED
#define HEADER_H_INCLUDED

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <sys/time.h>
#include <netdb.h>
#include <sys/unistd.h>

#define ERROR -1
#define MAX_CLIENTS 4
#define MAX_PEERS 3
#define MAX_DATA 1024
#define SERVER_MODE 1
#define CLIENT_MODE 2
#define MIN_PORT 1024
#define SERVER_NAME "timberlake.cse.buffalo.edu"
#define SERVER_IP "128.205.36.8"

struct connections
{
    int id;
    int sock_fd;
    char hostname[MAX_DATA];
    char ipaddr[MAX_DATA];
    int port;
};

#endif // HEADER_H_INCLUDED



