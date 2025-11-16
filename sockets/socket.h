#ifndef __SOCKET_H__
#define __SOCKET_H__

#ifdef __linux__
	#include <sys/socket.h>
	#include <sys/types.h>
	#include <sys/time.h>
	#include <unistd.h>
	#include <signal.h>
	#include <arpa/inet.h>
	#include <stdarg.h>
	#include <errno.h>
	#include <fcntl.h>
	#include <sys/ioctl.h>
	#include <netdb.h>
#endif

#define SERVER_PORT 80
#define MAXLINE 4096


typedef struct _socket
{
	int fd, n;
	struct sockaddr_in servaddr;
	char recv_line[MAXLINE];
} socket_t;

void tcpclient_init(int,char**,socket_t*);
void tcpclient_connect_server(socket_t*);
void tcpclient_send_server(socket_t*,char*);
void net_error(char *format, ...);

#endif // __SOCKET_H__
