#ifndef __SERVER_H__
#define __SERVER_H__

#ifdef __linux__
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
#elif _WIN64
	#include <winsock2.h>
	#include <Windows.h>

#endif 

#include "include/utils.h"

typedef struct _server
{
#ifdef __linux__
	int socket, n, client;
#elif _WIN64
	SOCKET win_socket, win_client;
#endif
	struct sockaddr_in servaddr;
	char buf[MAXLINE+1];
	char recvline[MAXLINE+1];
} server_t;

void server_init(server_t*)
void server_connect(server_t*);
void server_send(server_t*);
void win_server_init(server_t*); 

#endif // __SERVER_H__
