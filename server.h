#ifndef __SERVER_H__
#define __SERVER_H__

#include <sys/socket.h>
#include <sys/types.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <stdarg.h>
#include <errno.h>
#include <sys/time.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <poll.h>


#include "include/utils.h"

typedef struct _server
{
	int socket, n, client;
	struct sockaddr_in servaddr;
	char buf[MAXLINE+1];
	char recvline[MAXLINE+1];
} server_t;

void server_init(server_t*);
void server_connect(server_t*);
void server_send(server_t*);


#endif // __SERVER_H__
